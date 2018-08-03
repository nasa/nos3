# script to provide a COSMOS interface to a raw file of binary telemetry data

require 'cosmos/interfaces/interface'

module Cosmos

  # Base class for interfaces that send messages from a raw file of binary telemetry data
  class FileInterface < Interface

    # @param filename [String] Name of file containing binary telemetry data
    # @param pausemilliseconds [Integer] Number of milliseconds to pause between reads
    # LENGTH 64 16 11 1 BIG_ENDIAN 4 0xDEAD
    def initialize(filename, pause_milliseconds, stream_protocol_type, 
	length_bit_offset, length_bit_size, length_value_offset, bytes_per_count, length_endianness, 
	discard_leading_bytes, sync_pattern)
      super()
      @filename = filename
      @pause_milliseconds = pause_milliseconds.to_i
      if (@pause_milliseconds < 1) then
        puts "FileInterface: Invalid @pause_milliseconds=#{@pause_milliseconds}.  Valid range is 0 < x.  Setting value=1000."
        @pause_milliseconds = 1000
      end
      raise "FileInterface: Unsupported stream protocol #{stream_protocol_type}.  The only supported type is \"LENGTH\"." unless (stream_protocol_type == "LENGTH")
      @length_bit_offset = length_bit_offset.to_i
      if ((@length_bit_offset < 0) || (@length_bit_offset > 1600) || (@length_bit_offset % 8 != 0)) then
        puts "FileInterface: Invalid @length_bit_offset=#{@length_bit_offset}.  Valid range is multiples of 8 where 0 <= x <= 1600.  Setting value=64."
        @length_bit_offset = 64 
      end
      @length_bit_size = length_bit_size.to_i
      if ((@length_bit_size < 1) || (@length_bit_size > 256) || (@length_bit_size % 8 != 0)) then
        puts "FileInterface: Invalid @length_bit_size=#{@length_bit_size}.  Valid range is multiples of 8 where 0 < x <= 256.  Setting value=16."
        @length_bit_size = 16
      end
      @length_value_offset = length_value_offset.to_i
      if ((@length_value_offset < -100) || (@length_value_offset > 100)) then
        puts "FileInterface: Invalid @length_value_offset=#{@length_value_offset}.  Valid range is -100 <= x <= 100.  Setting value=11."
        @length_value_offset = 11
      end
      @bytes_per_count = bytes_per_count.to_i
      if ((@bytes_per_count < 1) || (@bytes_per_count > 10)) then
        puts "FileInterface: Invalid @bytes_per_count=#{@bytes_per_count}.  Valid range is 0 < x <= 10.  Setting value=1."
        @bytes_per_count = 1
      end
      raise "FileInterface: Unsupported length endianess #{length_endianness}.  The only supported type is \"BIG_ENDIAN\"." unless (length_endianness == "BIG_ENDIAN")
      @discard_leading_bytes = discard_leading_bytes.to_i
      if ((@discard_leading_bytes < 0) || (@discard_leading_bytes > 100)) then
        puts "FileInterface: Invalid @discard_leading_bytes=#{@discard_leading_bytes}.  Valid range is 0 <= x <= 100.  Setting value=4."
        @discard_leading_bytes = 4
      end
      @sync_array = parse_sync(sync_pattern)
      raise "FileInterface: Unable to process sync pattern #{sync_pattern}.  Valid sync patterns must have the form 0xHHHH (where any number of hex characters H can be used)." unless (@sync_array.length > 0)
      @connected = false
    end

    # Opens the binary telemetry file for reading
    def connect
      begin
        @file = File.open(@filename, "rb")
        @connected = true
      rescue SystemCallError
        @connected = false
      end
    end

    # @return [Boolean] Whether the file is open for reading 
    def connected?
      @connected
    end

    # Close the binary telemetry file for reading
    def disconnect
      @file.close unless @file.nil? or @file.closed?
      @connected = false
    end

    # The next chunk of bytes is read from the binary telemetry file and the data returned
    # in a {Packet}. bytes_read and read_count are updated.
    #
    # @return [Packet]
    def read
      sleep(@pause_milliseconds / 1000.0)
      data = read_packet()
      if (data == nil) then
        disconnect()
      else
        @bytes_read += data.length
        @read_count += 1
        return Packet.new(nil, nil, :BIG_ENDIAN, nil, data)
      end
    end

    def read_packet
      # puts "Reading next packet" # debug
      num_matched_sync_chars = 0
      unknown = ""
      sync_chars = ""
      sync_found = false
      while (byte = @file.read(1)) do
        if (@sync_array[num_matched_sync_chars].chr == byte) then
          # puts "Sync char found: #{byte}" # debug
          sync_chars += byte
          num_matched_sync_chars = num_matched_sync_chars + 1
          if (num_matched_sync_chars >= @sync_array.length) then
            sync_found = true
            break
          end
        else
          unknown += sync_chars
          unknown += byte
          sync_chars = ""
          num_matched_sync_chars = 0
        end
      end
      if (unknown.length > 0) then
        if ((unknown.length >= 2) && (unknown.getbyte(0) == 0xCC) && (unknown.getbyte(1) == 0xCC)) then
            # drop the fill from STF1
        else
            puts "ERROR: Sync not found.  Discarding #{unknown.length} bytes of data."
            puts "ERROR: Starting #{unknown[0..5]}"
        end
      end

      data = nil 
      if (sync_found) then
        data = sync_chars
        first_length_byte = (@length_bit_offset / 8).to_i
        last_length_byte = ((@length_bit_offset + @length_bit_size) / 8).to_i + (((@length_bit_offset + @length_bit_size) % 8 == 0) ? 0 : 1)
        length_bytes_to_read = last_length_byte - data.length
        data += @file.read(length_bytes_to_read)
        length_bytes = data[first_length_byte..last_length_byte]
        # Need to mask off the bits in length_bytes that are not part of the length... not doing that currently since for our purposes length always occupies all bits in a byte 
        length = length_bytes.unpack('n').first
        num_bytes_in_packet = (length + @length_value_offset) * @bytes_per_count
        #puts "first_length_byte=#{first_length_byte}" # debug
        #puts "last_length_byte=#{last_length_byte}" # debug
        #puts "length_bytes_to_read=#{length_bytes_to_read}" # debug
        #puts "length_bytes=#{length_bytes}" # debug
        #puts "length=#{length}" # debug
        #puts "num_bytes_in_packet=#{num_bytes_in_packet}" # debug
        #puts "@discard_leading_bytes=#{@discard_leading_bytes}" # debug
        #puts "before read for length: data.length=#{data.length}" # debug
        data += @file.read(num_bytes_in_packet - data.length)
        #puts "after read for length: data.length=#{data.length}" # debug
        data = data[@discard_leading_bytes..-1]
        #puts "after discarding bytes: data.length=#{data.length}" # debug
        #line = data.map { |b| sprintf(", 0x%02X",b) }.join
        line = data.unpack('H*')
        #puts "Data: #{line}" # debug
      end
      return data
    end

    def parse_sync(sync_pattern)
      retval = []
      if ((sync_pattern[0] == '0') && (sync_pattern[1] == 'x') && ((sync_pattern.length % 2) == 0) && (sync_pattern[2..sync_pattern.length] =~ /^[0-9A-F]+$/i)) then
        for i in 1..((sync_pattern.length/2)-1)
          retval.push(sync_pattern[2*i].to_i(16) * 16 + sync_pattern[2*i+1].to_i(16))
        end
      end
      return retval
    end

  end # class FileInterface

end # module Cosmos
