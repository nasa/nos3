# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos/config/config_parser'
require 'thread'

module Cosmos

  # Processes a {Stream} on behalf of an {Interface}. A {Stream} is a
  # primative interface that simply reads and writes raw binary data. The
  # StreamProtocol adds higher level processing including the ability to
  # discard a certain number of bytes from the stream and to sync the stream
  # on a given synchronization pattern. The StreamProtocol operates at the
  # {Packet} abstraction level while the {Stream} operates on raw bytes.
  class StreamProtocol

    # @return [Integer] The number of bytes read from the stream
    attr_accessor :bytes_read
    # @return [Integer] The number of bytes written to the stream
    attr_accessor :bytes_written
    # @return [Interface] The interface associated with this
    #   StreamProtocol. The interface is a higher level abstraction and is
    #   passed down to the StreamProtocol to allow it to call the callbacks in
    #   the interface when processing the {Stream}. otherwise subclass wins
    #   when calling
    attr_reader :interface
    # @return [Stream] The stream this StreamProtocol is processing data from
    attr_reader :stream

    # @return [Proc] The name of a method in the {Interface} that #read calls
    #   after reading data from the {Stream}. It should take a String binary data
    #   buffer and return a String binary data buffer.
    attr_accessor :post_read_data_callback
    # @return [Proc] The name of a method in the {Interface} that #read calls
    #   after converting the read data buffer into a {Packet}. It should take a
    #   {Packet} and return a {Packet}.
    attr_accessor :post_read_packet_callback
    # @return [Proc] The name of a method in the {Interface} that #write
    #   calls before writing the data to the stream.
    #   It should take a {Packet} and return a binary String buffer.
    attr_accessor :pre_write_packet_callback
    # @return [Proc] The name of a method in the {Interface} that #write
    #   calls after writing the data to the stream.  It should take a {Packet} and a String
    #   and return nothing.
    attr_accessor :post_write_data_callback

    # @param discard_leading_bytes [Integer] The number of bytes to discard
    #   from the binary data after reading from the {Stream}. Note that this is often
    #   used to remove a sync pattern from the final packet data.
    # @param sync_pattern [String] String representing a hex number ("0x1234")
    #   that will be searched for in the raw {Stream}. Bytes encountered before
    #   this pattern is found are discarded.
    # @param fill_sync_pattern [Boolean] Fill the sync pattern when writing packets
    def initialize(discard_leading_bytes = 0, sync_pattern = nil, fill_sync_pattern = false)
      @discard_leading_bytes = discard_leading_bytes.to_i
      @sync_pattern = ConfigParser.handle_nil(sync_pattern)
      @sync_pattern = @sync_pattern.hex_to_byte_string if @sync_pattern
      @fill_sync_pattern = ConfigParser.handle_true_false(fill_sync_pattern)

      @stream = nil
      @data = ''
      @bytes_read = 0
      @bytes_written = 0

      @interface = nil
      @post_read_data_callback = nil
      @post_read_packet_callback = nil
      @pre_write_packet_callback = nil
      @post_write_data_callback = nil
      @write_mutex = Mutex.new
    end

    # @param interface [Interface] Sets the higher level interface which is
    #   using this StreamProtocol. If the interface defines post_read_data,
    #   post_read_packet, or pre_write_packet, then these methods will be
    #   called over any subclass implementations.
    def interface=(interface)
      @interface = interface
      if @interface.respond_to? :post_read_data
        @post_read_data_callback = @interface.method(:post_read_data)
      end
      if @interface.respond_to? :post_read_packet
        @post_read_packet_callback = @interface.method(:post_read_packet)
      end
      if @interface.respond_to? :pre_write_packet
        @pre_write_packet_callback = @interface.method(:pre_write_packet)
      end
      if @interface.respond_to? :post_write_data
        @post_write_data_callback = @interface.method(:post_write_data)
      end
    end

    # @param stream [Stream] The stream this stream protocol should read and
    #   write to
    def connect(stream)
      @data = ''
      @data.force_encoding('ASCII-8BIT')
      @stream = stream
      @stream.connect
    end

    # @return [Boolean] Whether the stream attribute has been set and is
    #   connected
    def connected?
      if @stream
        @stream.connected?
      else
        false
      end
    end

    # Disconnects from the underlying {Stream} by calling {Stream#disconnect}.
    # Clears the data attribute.
    def disconnect
      @stream.disconnect if @stream
      @data = ''
    end

    # Reads from the stream. It can look for a sync pattern before
    # creating a Packet. It can discard a set number of bytes at the beginning
    # of the stream before creating the Packet.
    #
    # If the post_read_data_callback is defined (post_read_data is
    # implemented by the interface) then it is called to translate the
    # raw data. Otherwise post_read_data is called which does nothing
    # unless it is implemented by a subclass.
    #
    # @return [Packet|nil] A Packet of consisting of the bytes read from the
    #   stream.
    def read
      # Loop until we have a packet to give
      loop do
        result = handle_sync_pattern()
        return nil unless result

        # Reduce the data to a single packet
        packet_data = reduce_to_single_packet()
        return nil unless packet_data

        # Discard leading bytes if necessary
        packet_data.replace(packet_data[@discard_leading_bytes..-1]) if @discard_leading_bytes > 0

        # Return data based on final_receive_processing
        if @post_read_data_callback
          packet_data = @post_read_data_callback.call(packet_data)
        else
          packet_data = post_read_data(packet_data)
        end
        if packet_data
          if packet_data.length > 0
            # Valid packet
            packet = Packet.new(nil, nil, :BIG_ENDIAN, nil, packet_data)
            if @post_read_packet_callback
              packet = @post_read_packet_callback.call(packet)
            else
              packet = post_read_packet(packet)
            end
            return packet
          else
            # Packet should be ignored
            next
          end
        else
          # Connection lost
          return nil
        end
      end # loop do
    end

    # Writes the packet data to the stream.
    #
    # If the pre_write_packet_callback is defined (pre_write_packet is
    # implemented by the interface) then that is called to translate the
    # packet into data. Otherwise pre_write_packet is called which
    # returns the packet buffer unless it is implemented by a subclass.
    #
    # @param packet [Packet] Packet data to write to the stream
    def write(packet)
      @write_mutex.synchronize do
        if @pre_write_packet_callback
          data = @pre_write_packet_callback.call(packet)
        else
          data = pre_write_packet(packet)
        end
        if data
          write_raw(data, false)
          if @post_write_data_callback
            @post_write_data_callback.call(packet, data)
          else
            post_write_data(packet, data)
          end
        else
          # write aborted - don't write data
        end
      end
    end

    # Writes the raw binary string to the stream.
    #
    # @param data [String] Raw binary string
    # @param take_mutex [Boolean] Whether or not to take the write_mutex
    def write_raw(data, take_mutex = true)
      @write_mutex.lock if take_mutex
      begin
        if connected?()
          @stream.write(data)
          @bytes_written += data.length
        else
          raise "Stream not connected for write_raw"
        end
      ensure
        @write_mutex.unlock if take_mutex
      end
    end

    # Called to perform modifications on read data before making it into a packet
    #
    # @param packet_data [String] Raw packet data
    # @return [String] Potentially modified packet data
    def post_read_data(packet_data)
      packet_data
    end

    # Called to perform modifications on a read packet before it is given to the user
    #
    # @param packet [Packet] Original packet
    # @return [Packet] Potentially modified packet
    def post_read_packet(packet)
      packet
    end

    # Called to perform modifications on write data before writing it out the stream
    #
    # @param packet [Packet] packet to write out
    # @return [String] Potentially modified packet data
    def pre_write_packet(packet)
      data = packet.buffer(false)
      if @fill_sync_pattern
        # Put leading bytes back on
        data = ("\x00" * @discard_leading_bytes) << data if @discard_leading_bytes > 0

        # Fill the sync pattern
        if @sync_pattern
          BinaryAccessor.write(@sync_pattern,
            0,
            @sync_pattern.length * 8,
            :BLOCK,
            data,
            :BIG_ENDIAN,
            :ERROR)
        end
      end
      data
    end

    # Called to perform actions after writing data to the stream
    #
    # @param packet [Packet] packet that was written out
    # @param data [String] binary data that was written out
    def post_write_data(packet, data)
      # Default do nothing
    end

    protected

    # @return [Boolean] Whether we successfully found a sync pattern
    def handle_sync_pattern
      if @sync_pattern
        loop do
          # Make sure we have some data to look for a sync word in
          read_minimum_size(@sync_pattern.length)
          return false if @data.length <= 0

          # Find the beginning of the sync pattern
          sync_index = @data.index(@sync_pattern.getbyte(0).chr)
          if sync_index
            # Make sure we have enough data for the whole sync pattern past this index
            read_minimum_size(sync_index + @sync_pattern.length)
            return false if @data.length <= 0

            # Check for the rest of the sync pattern
            found = true
            index = sync_index
            @sync_pattern.each_byte do |byte|
              if @data.getbyte(index) != byte
                found = false
                break
              end
              index += 1
            end

            if found
              if sync_index != 0
                discard_length = @data[0..(sync_index - 1)].length
                log_discard(discard_length, true)
                # Delete Data Before Sync Pattern
                @data.replace(@data[sync_index..-1])
              end
              return true

            else # not found
              log_discard(@data[0..sync_index].length, false)
              # Delete Data Before and including first character of suspected sync Pattern
              @data.replace(@data[(sync_index + 1)..-1])
              next
            end # if found

          else # sync_index = nil
            log_discard(@data.length, false)
            @data.replace('')
            next
          end # unless sync_index.nil?
        end # end loop
      end # if @sync_pattern

      true
    end

    def log_discard(length, found)
      if ((@data.length >= 2) && (@data.getbyte(0) == 0xCC) && (@data.getbyte(1) == 0xCC))
        # drop the fill from STF1
      elsif ((@data.length == 7) && (@data.getbyte(0) == 0x48) && (@data.getbyte(1) == 0x65) && (@data.getbyte(2) == 0x6C) && (@data.getbyte(3) == 0x6C))
        # drop the HELLO
      else
        Logger.error("Sync #{'not ' unless found}found. Discarding #{length} bytes of data.")
        if @data.length >= 6
          Logger.error(sprintf("Starting: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            @data.getbyte(0), @data.getbyte(1), @data.getbyte(2), @data.getbyte(3), @data.getbyte(4), @data.getbyte(5)))
        end
      end
    end

    def reduce_to_single_packet
      if @data.length <= 0
        # Need to get some data
        read_and_handle_timeout()
        return nil if @data.length <= 0
      end

      # Reduce to packet data and clear data for next packet
      packet_data = @data.clone
      @data.replace('')

      packet_data
    end

    def read_and_handle_timeout
      begin
        data = @stream.read
        @bytes_read += data.length
      rescue Timeout::Error
        Logger.instance.error "Timeout waiting for data to be read"
        data = ''
      end
      # data.length == 0 means that the stream was closed.  Need to clear out @data and be done.
      if data.length == 0
        @data.replace('')
        return
      end
      @data << data
    end

    def read_minimum_size(required_num_bytes)
      while (@data.length < required_num_bytes)
        read_and_handle_timeout()
        return if @data.length <= 0
      end
    end

  end # class StreamProtocol

end # module Cosmos
