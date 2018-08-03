# encoding: ascii-8bit

require 'cosmos'
require 'cosmos/utilities/crc'
require 'cosmos/interfaces/tcpip_client_interface'

module Cosmos

  class TcpipClientInterfaceWrapper < TcpipClientInterface
    @@message_count = 0

    # Passthrough
    def initialize(hostname,
                   write_port,
                   read_port,
                   write_timeout,
                   read_timeout,
                   stream_protocol_type,
                   *stream_protocol_args)
      super(hostname,
                   write_port,
                   read_port,
                   write_timeout,
                   read_timeout,
                   stream_protocol_type,
                   *stream_protocol_args)
    end

    # Starts the raw logging interface, then is a passthrough
    def connect
      start_raw_logging_interface(@name) # The reason for this script existing
      super
    end

    def pre_write_packet(packet)
      data = packet.buffer(false) # Retrieve the raw buffer from the out bound packet.
      # If there is a CCSDS_CRC value attached, calculate and add before sending
      if packet.items['MSG_COUNT']
        data[28] = [@@message_count & 0xFF].pack('C')
        @@message_count += 1
      end
      # If there is a CCSDS_CRC value attached, calculate and add before sending
      #if packet.items['CCSDS_CHECKSUM']
      #  crc16 = Cosmos::Crc16.new
      #  new_crc = crc16.calculate_crc16(data[29..-3]) & 0xFFFF # Caculate CRC-16 on every byte, minus header 19 bytes, CRC bytes 2 - bytes
      #  data[-2] = [new_crc >> 8 & 0xFF].pack('C')
      #  data[-1] = [new_crc & 0xFF].pack('C')
      #end
      return data
    end

  end # class TcpipClientInterfaceWrapper

end # module Cosmos
