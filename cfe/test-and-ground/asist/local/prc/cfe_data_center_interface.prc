PROC cfe_data_center_interface

global to_cmd_sent

   DIRECTIVE open_tlm IS
      BEGIN
        dc_dir open udp_read C%03X 1235 packet
      END

   DIRECTIVE open_tlm_only IS
      BEGIN
        dc_dir open udp_read C%03X 1235 packet_only
      END

   DIRECTIVE close_tlm IS
      BEGIN
        dc_dir close C%03X
      END

  DIRECTIVE open_cmd(what_machine) IS
      BEGIN
        global to_cmd_sent
        local what_machine_no
        local pktid, seqctr, ctr0
    
        what_machine = %lower(what_machine)

        IF (what_machine = gsescstatval14) THEN
          write "<!> CMD path is already ",gsescstatval14,". No action taken."
          return
        ENDIF
    
        IF (what_machine <> "cdh" and what_machine <> "pse" and ;;
            what_machine <> "ace" and what_machine <> "comm" and ;;
            what_machine <> "cpu1" and what_machine <>"cpu2" and ;;
            what_machine <> "cpu3" and ;;
            what_machine <> "none" ) THEN   
      
          write "<!> Invalid CMD path value ",what_machine,". No action taken."
          return
        ENDIF

        IF ( gsescstatval14 <> "none" and ;;if
             %length(gsescstatval14) <> 0 ) THEN   
      
          write "<!> Type close_cmd first. No action taken."
          return
        ENDIF


        /ddir data="open udp_write CPKT " & what_machine & " 1234 ascii_pkt"
        gsescstatval14 = what_machine

        if (what_machine = "cdh") or (what_machine = "cpu1")  then
           what_machine_no = "1"
           pktid = 0
        elseif (what_machine = "pse") or (what_machine = "cpu2") then
           what_machine_no = "2"
           pktid = 32
        elseif (what_machine = "ace") or (what_machine = "cpu3") then
           what_machine_no = "3"
           pktid = 64
        endif

        ; if (to_cmd_sent <> what_machine_no) then
          s send_that_to_command
          seqctr = "P"&%hex(pktid,3)&"SCNT"
          ctr0 = %name(seqctr)
          wait until (ctr0 != %name(seqctr) or (%gmt+20))
        ; wait 20
        ; endif
        s fill_in_spacecraft_status

      END

  DIRECTIVE close_cmd IS
     BEGIN
        dc_dir close CPKT
        s clear_spacecraft_status
        gsescstatval14 = "none"
     END

  DIRECTIVE close_data_center IS
      BEGIN
         gsescstatval14 = "none"
         to_cmd_sent = 0
         s clear_spacecraft_status
         dc_close
      END
  acq i on
ENDPROC
