
Note that the unit test could not be run properly with the save-to-CDS functionality turned on. When trying to register 256 RTS tables as Critical, ES returns an error because too many CDS items were defined (which TBL uses to make tables Critical). The configuration parameter currently has an upper bound of 128, which is way less than this test needs. ES code will have to re-written for this test to work. A DCR has been entered into MKS against CFE ES.  DCR 6785.

Overall Coverage Statistics
-----------------------------------

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_app.c'
Lines executed:99.33% of 149
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_app.c:creating 'sc_app.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_atsrq.c'
Lines executed:100.00% of 143
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_atsrq.c:creating 'sc_atsrq.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_cmds.c'
Lines executed:100.00% of 256
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_cmds.c:creating 'sc_cmds.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_loads.c'
Lines executed:97.87% of 141
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_loads.c:creating 'sc_loads.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_rtsrq.c'
Lines executed:100.00% of 75
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_rtsrq.c:creating 'sc_rtsrq.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_state.c'
Lines executed:100.00% of 70
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_state.c:creating 'sc_state.c.gcov'

File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_utils.c'
Lines executed:100.00% of 30
/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_utils.c:creating 'sc_utils.c.gcov'

---------------------------------------------------------------------------------------


Comments on Functions with less than 100% Coverage
-----------------------------------------------
File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_app.c'
Lines executed:99.33% of 149


        -: 1104:void SC_ExitApp(void)
        2: 1105:{
        2: 1106:    boolean AllSaved = TRUE;
        -: 1107:
        -: 1108:    /*
        -: 1109:    ** If we're using the CDS, try to update its data
        -: 1110:    */
        -: 1111:#ifdef SC_SAVE_TO_CDS
		... ... ...
                ... ... ...
        -: 1175:#else
        2: 1176:    AllSaved = FALSE;
        -: 1177:#endif
        -: 1178:
        -: 1179:
        2: 1180:    if (AllSaved == TRUE)
        -: 1181:    {
    #####: 1182:        CFE_EVS_SendEvent(SC_APP_SAVED_INF_EID,
        -: 1183:                          CFE_EVS_INFORMATION,
        -: 1184:                          "Application Data and Tables saved on exit");
        -: 1185:    }

Because this test was run without the SC_SAVE_TO_CDS parameter on, so it is impossible to get inside the if statement.




File '/Users/nyanchik/Desktop/cfs-sandbox/sc/fsw/src/sc_loads.c'
Lines executed:97.87% of 141



        -:  123:void SC_LoadAts (uint16 AtsId)
       11:  124:{
        -:  125:    uint16                  AtsCmdLength;       /* the length of the command in words     */
        -:  126:    uint16                  AtsCmdNum;          /* the current command number in the load */
        -:  127:    uint16                  AtsCmdPtr;          /* the current command pointer in the load */
        -:  128:    CFE_SB_MsgPtr_t         AtsCmd;             /* a pointer to an ats command */
        -:  129:    SC_AtsCommandHeader_t  *AtsCmdHeaderPtr;    /* a pointer to the ats commandwith the ats header */
        -:  130:    uint16                  AtsLoadStatus;      /* the status of the ats load in progress */
        -:  131:    uint16                 *AtsTablePtr;        /* pointer to the start of the Ats table */
        -:  132:
        -:  133:
        -:  134:    /*
        -:  135:     ** Initialize all structrures
        -:  136:     */
       11:  137:    SC_InitAtsTables (AtsId);
        -:  138: 
       11:  139:    AtsTablePtr = SC_OperData.AtsTblAddr[AtsId];
        -:  140:    
        -:  141:    /* initialize the pointers and counters */
       11:  142:    AtsCmdPtr = 0;
       11:  143:    AtsLoadStatus = SC_PARSING;
        -:  144:  
      965:  145:        while (AtsLoadStatus == SC_PARSING)
        -:  146:    {
        -:  147:        /*
        -:  148:         ** Make sure that the pointer as well as the primary packet
        -:  149:         ** header fit in the buffer, so a G.P fault is not caused.
        -:  150:         */
      943:  151:        if (AtsCmdPtr < SC_ATS_BUFF_SIZE)
        -:  152:        {
        -:  153:            /* get the next command number from the buffer */
      942:  154:            AtsCmdNum = ((SC_AtsCommandHeader_t *)&AtsTablePtr[AtsCmdPtr]) ->CmdNum;
        -:  155:    
      942:  156:            if (AtsCmdNum == 0)
        -:  157:            {   
        -:  158:                /* end of the load reached */
        6:  159:                AtsLoadStatus = SC_COMPLETE;
        -:  160:            }
        -:  161:           
        -:  162:                    /* make sure the CmdPtr can fit in a whole Ats Cmd Header at the very least */
      936:  163:            else if (AtsCmdPtr > (SC_ATS_BUFF_SIZE - (sizeof(SC_AtsCommandHeader_t)/SC_BYTES_IN_WORD)))
        -:  164:            {
        -:  165:                /*
        -:  166:                **  A command does not fit in the buffer
        -:  167:                */
        1:  168:                AtsLoadStatus = SC_ERROR;
        -:  169:            }  /* else if the cmd number is valid and the command */
        -:  170:            /* has not already been loaded                     */
        -:  171:            else
      935:  172:                if (AtsCmdNum <= SC_MAX_ATS_CMDS &&
        -:  173:                    SC_OperData.AtsCmdStatusTblAddr[AtsId][AtsCmdNum - 1] == SC_EMPTY)
        -:  174:                {
        -:  175:                    /* get a pointer to the ats command in the table */
      934:  176:                    AtsCmdHeaderPtr = (SC_AtsCommandHeader_t*) (&AtsTablePtr[AtsCmdPtr]);
      934:  177:                    AtsCmd = (CFE_SB_MsgPtr_t)(AtsCmdHeaderPtr -> CmdHeader);
        -:  178:                                       
        -:  179:                    /* if the length of the command is valid */
      934:  180:                    if (CFE_SB_GetTotalMsgLength(AtsCmd) >= SC_PACKET_MIN_SIZE && 
        -:  181:                        CFE_SB_GetTotalMsgLength(AtsCmd) <= SC_PACKET_MAX_SIZE)
        -:  182:                    {
        -:  183:                        /* get the length of the command in WORDS */
      933:  184:                        AtsCmdLength = (CFE_SB_GetTotalMsgLength(AtsCmd) + SC_ATS_HEADER_SIZE) / SC_BYTES_IN_WORD; 
        -:  185:                        
        -:  186:                        /* if the command does not run off of the end of the buffer */
      933:  187:                        if (AtsCmdPtr + AtsCmdLength <= SC_ATS_BUFF_SIZE)
        -:  188:                        {
        -:  189:                            /* set the command pointer in the command index table */
        -:  190:                            /* CmdNum starts at one....                          */
        -:  191:                            
      932:  192:                            SC_AppData.AtsCmdIndexBuffer[AtsId] [AtsCmdNum -1] = 
        -:  193:                                                (SC_AtsCommandHeader_t *)(&SC_OperData.AtsTblAddr[AtsId][AtsCmdPtr]);
        -:  194:                            
        -:  195:                            /* set the command status to loaded in the command status table */
      932:  196:                            SC_OperData.AtsCmdStatusTblAddr[AtsId][AtsCmdNum - 1] = SC_LOADED;
        -:  197:                            
        -:  198:                            /* increment the number of commands loaded */
      932:  199:                            SC_OperData.AtsInfoTblAddr[AtsId].NumberOfCommands++;
        -:  200:                            
        -:  201:                            /* increment the ats_cmd_ptr to point to the next command */
      932:  202:                            AtsCmdPtr = AtsCmdPtr + AtsCmdLength;
        -:  203:                        }
        -:  204:                        else
        -:  205:                        { /* the command runs off the end of the buffer */
        1:  206:                              AtsLoadStatus = SC_ERROR;
        -:  207:                        } /* end if */
        -:  208:                    }
        -:  209:                    else
        -:  210:                    { /* the command length was invalid */
        1:  211:                        AtsLoadStatus = SC_ERROR;
        -:  212:                    } /* end if */
        -:  213:                }
        -:  214:                else
        -:  215:                { /* the cmd number is invalid */                    
        1:  216:                    AtsLoadStatus = SC_ERROR;
        -:  217:                } /* end if */
        -:  218:        }
        -:  219:        else
        -:  220:        {
        1:  221:            if (AtsCmdPtr == SC_ATS_BUFF_SIZE)
        -:  222:            {
        -:  223:                /* we encountered a load exactly as long as the buffer */
        1:  224:                AtsLoadStatus = SC_COMPLETE;
        -:  225:
        -:  226:            }
        -:  227:            else
        -:  228:            { /* the pointer is over the end of the buffer */
        -:  229:                
    #####:  230:                AtsLoadStatus = SC_ERROR;
        -:  231:            } /* end if */
        -:  232:        }/*end else */
        -:  233:    } /* end while */


Because the AtsPtr increments itself, and if it runs over the buffer size elsewhere in the code and it is detected, this line is near impossible to get to unless it gets a memory hit while in that part of the code



        -:  390:int32 SC_ValidateAts (void *TableData)
       12:  391:{
        -:  392:    uint16                           AtsCmdLength;       /* the length of the command in words     */
        -:  393:    uint16                           AtsCmdNum;          /* the current command number in the load */
        -:  394:    uint16                           AtsCmdPtr;          /* the current command pointer in the load */
       12:  395:    uint16                           NumberOfCommands = 0;
        -:  396:    uint16                           i;
        -:  397:    int32                            AtsLoadStatus;     /* the status of the ats load in progress */
        -:  398:    CFE_SB_MsgPtr_t                  AtsCmd;            /* a pointer to an ats command */
        -:  399:    SC_AtsCommandHeader_t *          AtsCmdHeaderPtr;
        -:  400:    uint16                          *AtsTable;
        -:  401:    static uint8                     AtsCmdStatusTblAddr[SC_MAX_ATS_CMDS];
        -:  402:    static SC_AtsCommandHeader_t    *AtsCmdIndexBuffer[SC_MAX_ATS_CMDS];
        -:  403:    
        -:  404:    
        -:  405:    /* initialize the pointers and counters */
       12:  406:    AtsCmdPtr = 0;
       12:  407:    AtsLoadStatus = SC_PARSING;
       12:  408:    AtsTable = TableData;
        -:  409:    
    30012:  410:    for (i = 0; i < SC_MAX_ATS_CMDS; i++)
        -:  411:    {
    30000:  412:        AtsCmdStatusTblAddr[i] = SC_EMPTY;
        -:  413:    }
        -:  414:   
      956:  415:    while (AtsLoadStatus == SC_PARSING)
        -:  416:    {
        -:  417:        
        -:  418:
        -:  419:        /*
        -:  420:         ** Make sure that the pointer as well as the primary packet
        -:  421:         ** header fit in the buffer.
        -:  422:         */
      944:  423:        if (AtsCmdPtr < SC_ATS_BUFF_SIZE)
        -:  424:        {            
        -:  425:            
        -:  426:            /* get the next command number from the buffer */
      943:  427:            AtsCmdNum = ((SC_AtsCommandHeader_t *)&AtsTable[AtsCmdPtr]) ->CmdNum;
        -:  428:                           
      943:  429:            if (AtsCmdNum == 0)
        -:  430:            {   /* end of the load reached */
        -:  431:                
        7:  432:                AtsLoadStatus = CFE_SUCCESS;
        -:  433:            }
        -:  434:                    /* make sure the CmdPtr can fit in a whole Ats Cmd Header at the very least */
      936:  435:            else if (AtsCmdPtr > (SC_ATS_BUFF_SIZE - (sizeof(SC_AtsCommandHeader_t)/SC_BYTES_IN_WORD)) ) 
        -:  436:            {
        -:  437:                
        -:  438:                /*
        -:  439:                 **  The command does not fit in the buffer
        -:  440:                 */
        1:  441:                AtsLoadStatus = SC_ERROR;
        1:  442:                CFE_EVS_SendEvent (SC_ATS_PARSE_TOO_BIG_ERR_EID,
        -:  443:                                   CFE_EVS_ERROR,
        -:  444:                                   "ATS Load Aborted: Cmd number %d at offset %d doesn't fit in ATS buffer",
        -:  445:                                   AtsCmdNum, 
        -:  446:                                   AtsCmdPtr);
        -:  447:
        -:  448:            }  /* else if the cmd number is valid and the command */
        -:  449:               /* has not already been loaded                     */
      935:  450:            else if (AtsCmdNum <= SC_MAX_ATS_CMDS &&
        -:  451:                     AtsCmdStatusTblAddr[AtsCmdNum - 1] == SC_EMPTY)
        -:  452:            {
        -:  453:                /* get a pointer to the ats command in the Table */   
        -:  454:                
      934:  455:                AtsCmdHeaderPtr = (SC_AtsCommandHeader_t *)&AtsTable[AtsCmdPtr];
      934:  456:                AtsCmd = (CFE_SB_MsgPtr_t) (AtsCmdHeaderPtr -> CmdHeader);
        -:  457:                
        -:  458:                /* if the length of the command is valid */
      934:  459:                if (CFE_SB_GetTotalMsgLength(AtsCmd) >= SC_PACKET_MIN_SIZE &&
        -:  460:                    CFE_SB_GetTotalMsgLength(AtsCmd) <= SC_PACKET_MAX_SIZE)
        -:  461:                { 
        -:  462:                    /* get the length of the command in WORDS */
      933:  463:                    AtsCmdLength = (CFE_SB_GetTotalMsgLength(AtsCmd) + SC_ATS_HEADER_SIZE) / SC_BYTES_IN_WORD;
        -:  464:                    
        -:  465:                    /* if the command does not run off of the end of the buffer */
      933:  466:                    if (AtsCmdPtr + AtsCmdLength <= SC_ATS_BUFF_SIZE)
        -:  467:                    {       
        -:  468:                        /* set the command pointer in the command index table */
        -:  469:                        /* CmdNum starts at one....                           */
      932:  470:                        AtsCmdIndexBuffer[AtsCmdNum -1]  = 
        -:  471:                        (SC_AtsCommandHeader_t *)&AtsTable[AtsCmdPtr];
        -:  472:                        
        -:  473:                        /* set the command status to loaded in the command status table */
      932:  474:                        AtsCmdStatusTblAddr[AtsCmdNum - 1] = SC_LOADED;
        -:  475:                        
        -:  476:                        /* increment the number of commands loaded */
      932:  477:                        NumberOfCommands++;
        -:  478:                        
        -:  479:                        /* increment the ats_cmd_ptr to point to the next command */
      932:  480:                        AtsCmdPtr = AtsCmdPtr + AtsCmdLength;
        -:  481:                                                
        -:  482:                    }
        -:  483:                    else
        -:  484:                    { /* the command runs off the end of the buffer */
        -:  485:                        
        1:  486:                        AtsLoadStatus = SC_ERROR;
        -:  487:                        
        1:  488:                        CFE_EVS_SendEvent (SC_ATS_PARSE_RUNS_OFF_ERR_EID,
        -:  489:                                           CFE_EVS_ERROR,
        -:  490:                                           "ATS Load Aborted: The length for cmd number %d at offset %d runs off the end of the ATS buffer",
        -:  491:                                           AtsCmdNum, 
        -:  492:                                           AtsCmdPtr);    
        -:  493:                    } /* end if */
        -:  494:                }
        -:  495:                else
        -:  496:                { /* the command length was invalid */
        -:  497:                    
        1:  498:                    AtsLoadStatus = SC_ERROR;
        1:  499:                    CFE_EVS_SendEvent (SC_ATS_PARSE_LEN_INVALID_ERR_EID,
        -:  500:                                       CFE_EVS_ERROR,
        -:  501:                                       "ATS Load Aborted: The length for command number %d at offset %d is invalid",
        -:  502:                                       AtsCmdNum, 
        -:  503:                                       AtsCmdPtr);    
        -:  504:                } /* end if */
        -:  505:            }
        -:  506:            else
        -:  507:            { /* the cmd number is invalid */
        -:  508:                
        1:  509:                AtsLoadStatus = SC_ERROR;
        1:  510:                CFE_EVS_SendEvent (SC_ATS_PARSE_CMD_INVALID_ERR_EID,
        -:  511:                                   CFE_EVS_ERROR,
        -:  512:                                   "ATS Load Aborted: The command number %d at offset %d is invalid",
        -:  513:                                   AtsCmdNum,
        -:  514:                                   AtsCmdPtr);  
        -:  515:            } /* end if */
        -:  516:        }
        1:  517:        else if (AtsCmdPtr == SC_ATS_BUFF_SIZE)
        -:  518:        {
        -:  519:            /* we encountered a load exactly as long as the buffer */
        1:  520:            AtsLoadStatus = CFE_SUCCESS;   
        -:  521:        }
        -:  522:        else
        -:  523:        { /* the pointer is over the end of the buffer */
        -:  524:            
    #####:  525:            AtsLoadStatus = SC_ERROR;
        -:  526:            
    #####:  527:            CFE_EVS_SendEvent (SC_ATS_PARSE_TOO_LONG_ERR_EID,
        -:  528:                               CFE_EVS_ERROR,
        -:  529:                               "ATS Load Aborted: Load is too long");
        -:  530:        } /* end if */
        -:  531:        
        -:  532:    } /* end while */

Because the AtsPtr increments itself, and if it runs over the buffer size elsewhere in the code and it is detected, this line is near impossible to get to unless it gets a memory hit while in that part of the code