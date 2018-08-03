require 'cosmos'
require 'cosmos/tools/data_viewer/data_viewer_component'

module Cosmos

    class CamExpTlmT < DataViewerComponent

        def initialize(parent, tab_name)
            super(parent, tab_name)
            @file = File.open(File.join(@log_file_directory, File.build_timestamped_filename(['CAM_Exp_tlm_t'], '.csv')), 'a+')
            @file.write("CCSDS_PKT_VER,CCSDS_PKT_TYP,CCSDS_SEC_FLG,CCSDS_APID,CCSDS_SEQ_FLAGS,CCSDS_SEQ_COUNT,CCSDS_LENGTH,CCSDS_SECONDS,CCSDS_SUBSECS,DATA,MSG_COUNT,\n")
        end

        def process_packet(packet)
            processed_text = ""
            processed_text += "%3.3b, " % packet.read('CCSDS_PKT_VER')
            processed_text += "%1.1b, " % packet.read('CCSDS_PKT_TYP')
            processed_text += "%1.1b, " % packet.read('CCSDS_SEC_FLG')
            processed_text += "%4.4x, " % packet.read('CCSDS_APID')
            processed_text += "%2.2b, " % packet.read('CCSDS_SEQ_FLAGS')
            processed_text += "%6.6d, " % packet.read('CCSDS_SEQ_COUNT')
            processed_text += "%6.6d, " % packet.read('CCSDS_LENGTH')
            processed_text += "%10.10d, " % packet.read('CCSDS_SECONDS')
            processed_text += "%6.6d, " % packet.read('CCSDS_SUBSECS')
            processed_text += "%s, " % (packet.read('CAM_DATA')).unpack('H*')
            processed_text += "%10.10u, " % packet.read('MSG_COUNT')
            
            # Picture Test
			fp = File.open('cam.jpg', 'ab+') 
			IO.binwrite(fp, (packet.read('CAM_DATA')), mode: 'a')
			fp.close
            
            if @processed_queue.length < 1000
               @processed_queue << processed_text
            end
            @file.write(processed_text + "\n")
            @file.flush
        end
    end
end
