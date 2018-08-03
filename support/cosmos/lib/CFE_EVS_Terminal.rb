require 'cosmos'
require 'cosmos/tools/data_viewer/data_viewer_component'

module Cosmos

    class CfeEvsTerminal < DataViewerComponent

        def initialize(parent, tab_name)
            super(parent, tab_name)
            @file = File.open(File.join(@log_file_directory, File.build_timestamped_filename(['CFE_EVS_Terminal'], '.csv')), 'a+')
            @file.write("CCSDS_SECONDS,CCSDS_SUBSECS,PACKETID_APPNAME,PACKETID_EVENTID,PACKETID_EVENTTYPE,MESSAGE,\n")
        end

        def process_packet(packet)
            processed_text =  ""
            processed_text += "%6.6d, " % packet.read('CCSDS_LENGTH')
            processed_text += "%10.10d, " % packet.read('CCSDS_SECONDS')
            processed_text += "%6.6d, " % packet.read('CCSDS_SUBSECS')
            processed_text += "%-10s, " % packet.read('PACKETID_APPNAME')
            processed_text += "%6.6u, " % packet.read('PACKETID_EVENTID')
            processed_text += "%6.6u, " % packet.read('PACKETID_EVENTTYPE')
            processed_text += "%-s, " % packet.read('MESSAGE').tr(',','')
            if @processed_queue.length < 1000
               @processed_queue << processed_text
            end
            @file.write(processed_text + "\n")
            @file.flush
        end
    end
end
