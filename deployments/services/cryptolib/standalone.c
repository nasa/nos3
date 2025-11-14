/* Copyright (C) 2009 - 2022 National Aeronautics and Space Administration.
   All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any kind, either expressed, implied, or statutory,
   including, but not limited to, any warranty that the software will conform to specifications, any implied warranties
   of merchantability, fitness for a particular purpose, and freedom from infringement, and any warranty that the
   documentation will conform to the program, or any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or
   consequential damages, arising out of, resulting from, or in any way connected with the software or its
   documentation, whether or not based upon warranty, contract, tort or otherwise, and whether or not loss was sustained
   from, or arose out of the results of, or use of, the software, documentation or services provided hereunder.

   ITC Team
   NASA IV&V
   jstar-development-team@mail.nasa.gov
*/

/*******************************************************************************
** Standalone CryptoLib Implementation
** UDP interfaces to apply / process each frame type and return the result.
*******************************************************************************/

#include "standalone.h"

/*
** Global Variables
*/
#define DYNAMIC_LENGTHS 1

static volatile uint8_t keepRunning    = CRYPTO_LIB_SUCCESS;
static volatile uint8_t tc_seq_num     = 0;
static volatile uint8_t tc_vcid        = CRYPTO_STANDALONE_FRAMING_VCID;
static volatile uint8_t tc_debug       = 1;
static volatile uint8_t tm_debug       = 0;
static volatile uint8_t crypto_use_tcp = STANDALONE_TCP ? 1 : 0;

/*
** Functions
*/
int32_t crypto_standalone_check_number_arguments(int actual, int expected)
{
    int32_t status = CRYPTO_LIB_SUCCESS;
    if (actual != expected)
    {
        status = CRYPTO_LIB_ERROR;
        printf("Invalid command format or number of arguments, type 'help' for more info\n");
    }
    return status;
}

void crypto_standalone_to_lower(char *str)
{
    char *ptr = str;
    while (*ptr)
    {
        *ptr = tolower((unsigned char)*ptr);
        ptr++;
    }
    return;
}

void crypto_standalone_print_help(void)
{
    printf(CRYPTO_PROMPT "command [args]\n"
                         "----------------------------------------------------------------------\n"
                         "exit                               - Exit app                         \n"
                         "help                               - Display help                     \n"
                         "noop                               - No operation command to device   \n"
                         "reset                              - Reset CryptoLib                  \n"
                         "active                             - Displays all operational SAs     \n"
                         "tc                                 - Toggle TC debug prints           \n"
                         "tm                                 - Toggle TM debug prints           \n"
                         "vcid #                             - Change active TC virtual channel \n"
                         "\n");
}

int32_t crypto_standalone_get_command(const char *str)
{
    int32_t status = CRYPTO_CMD_UNKNOWN;
    char    lcmd[CRYPTO_MAX_INPUT_TOKEN_SIZE];

    strncpy(lcmd, str, CRYPTO_MAX_INPUT_TOKEN_SIZE);
    crypto_standalone_to_lower(lcmd);

    if (strcmp(lcmd, "help") == 0)
    {
        status = CRYPTO_CMD_HELP;
    }
    else if (strcmp(lcmd, "exit") == 0)
    {
        status = CRYPTO_CMD_EXIT;
    }
    else if (strcmp(lcmd, "noop") == 0)
    {
        status = CRYPTO_CMD_NOOP;
    }
    else if (strcmp(lcmd, "reset") == 0)
    {
        status = CRYPTO_CMD_RESET;
    }
    else if (strcmp(lcmd, "vcid") == 0)
    {
        status = CRYPTO_CMD_VCID;
    }
    else if (strcmp(lcmd, "tc") == 0)
    {
        status = CRYPTO_CMD_TC_DEBUG;
    }
    else if (strcmp(lcmd, "tm") == 0)
    {
        status = CRYPTO_CMD_TM_DEBUG;
    }
    else if (strcmp(lcmd, "active") == 0)
    {
        status = CRYPTO_CMD_ACTIVE;
    }
    return status;
}

int32_t crypto_standalone_process_command(int32_t cc, int32_t num_tokens, char *tokens)
{
    int32_t status = CRYPTO_LIB_SUCCESS;

    /* Process command */
    switch (cc)
    {
        case CRYPTO_CMD_HELP:
            crypto_standalone_print_help();
            break;

        case CRYPTO_CMD_EXIT:
            keepRunning = CRYPTO_LIB_ERROR;
            break;

        case CRYPTO_CMD_NOOP:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                printf("NOOP command success\n");
            }
            break;

        case CRYPTO_CMD_RESET:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                status = crypto_reset();
                printf("Reset command received\n");
            }
            break;

        case CRYPTO_CMD_VCID:
            if (crypto_standalone_check_number_arguments(num_tokens, 1) == CRYPTO_LIB_SUCCESS)
            {
                uint8_t vcid = (uint8_t)atoi(&tokens[0]);
                /* Confirm new VCID valid */
                if (vcid < 64)
                {
                    SaInterface            sa_if            = get_sa_interface_inmemory();
                    SecurityAssociation_t *test_association = NULL;
                    int32_t                status           = CRYPTO_LIB_SUCCESS;

                    status = sa_if->sa_get_operational_sa_from_gvcid(0, SCID, vcid, 0, &test_association);
                    if (status == CRYPTO_LIB_SUCCESS)
                    {
                        Crypto_saPrint(test_association);
                    }
                    printf("Get_SA_Status: %d\n", status);
                    if ((status == CRYPTO_LIB_SUCCESS) && (test_association->sa_state == SA_OPERATIONAL) &&
                        (test_association->gvcid_blk.mapid == TYPE_TC) && (test_association->gvcid_blk.scid == SCID))
                    {
                        tc_vcid = vcid;
                        printf("Changed active virtual channel (VCID) to %d \n", tc_vcid);
                    }
                    else
                    {
                        printf("Error - virtual channel (VCID) %d is invalid! Sticking with prior vcid %d \n", vcid,
                               tc_vcid);
                        status = CRYPTO_LIB_SUCCESS;
                    }
                }
                else
                {
                    printf("Error - virtual channel (VCID) %d must be less than 64! Sticking with prior vcid %d \n",
                           vcid, tc_vcid);
                }
            }
            break;

        case CRYPTO_CMD_TC_DEBUG:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                if (tc_debug == 0)
                {
                    tc_debug = 1;
                    printf("Enabled TC debug prints! \n");
                }
                else
                {
                    tc_debug = 0;
                    printf("Disabled TC debug prints! \n");
                }
            }
            break;

        case CRYPTO_CMD_TM_DEBUG:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                if (tm_debug == 0)
                {
                    tm_debug = 1;
                    printf("Enabled TM debug prints! \n");
                }
                else
                {
                    tm_debug = 0;
                    printf("Disabled TM debug prints! \n");
                }
            }
            break;

        case CRYPTO_CMD_ACTIVE:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                SaInterface            sa_if            = get_sa_interface_inmemory();
                SecurityAssociation_t *test_association = NULL;

                printf("Active SAs: \n\t");
                for (int i = 0; i < NUM_SA; i++)
                {
                    sa_if->sa_get_from_spi(i, &test_association);
                    if (test_association->sa_state == SA_OPERATIONAL)
                    {
                        if (i < 5)
                        {
                            printf("TC  - ");
                        }
                        if (i > 4 && i < 9)
                        {
                            printf("TM  - ");
                        }
                        if (i > 8 && i < 13)
                        {
                            printf("AOS - ");
                        }
                        if (i == 63)
                        {
                            printf("ExProc - ");
                        }

                        printf("SPI %d - VCID %d - EST %d - AST %d\n\t", i, test_association->gvcid_blk.vcid,
                               test_association->est, test_association->ast);
                    }
                }
                printf("\n");
            }
            break;

        default:
            printf("Invalid command format, type 'help' for more info\n");
            status = CRYPTO_LIB_ERROR;
            break;
    }

    return status;
}

int32_t crypto_host_to_ip(const char *hostname, char *ip)
{
    struct addrinfo hints, *res, *p;
    int             status;
    void           *addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET; // Uses IPV4 only.  AF_UNSPEC for IPV6 Support
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr                     = &(ipv4->sin_addr);

        // Convert IP to String
        if (inet_ntop(p->ai_family, addr, ip, INET_ADDRSTRLEN) == NULL)
        {
            freeaddrinfo(res);
            return 1;
        }

        freeaddrinfo(res);
        return 0; // IP Found
    }
    freeaddrinfo(res);
    return 1; // IP NOT Found
}

int32_t crypto_standalone_socket_init(udp_info_t *sock, int32_t port, uint8_t bind_sock, int connection)
{
    int       status = CRYPTO_LIB_SUCCESS;
    int       optval;
    socklen_t optlen;

    sock->port = port;

    if (connection == 1)
    {
        /* Creating TCP socket */
        sock->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (sock->sockfd == -1)
        {
            printf("tcp_init: Socket create error on port %d\n", sock->port);
            return CRYPTO_LIB_ERROR;
        }

        /* Determine IP */
        sock->saddr.sin_family = AF_INET;
        if (inet_addr(sock->ip_address) != INADDR_NONE)
        {
            sock->saddr.sin_addr.s_addr = inet_addr(sock->ip_address);
        }
        else
        {
            char ip[16];
            int  check = crypto_host_to_ip(sock->ip_address, ip);
            if (check == 0)
            {
                sock->saddr.sin_addr.s_addr = inet_addr(ip);
            }
            else
            {
                printf("socket_init: Failed to resolve hostname '%s'\n", sock->ip_address);
                return CRYPTO_LIB_ERROR;
            }
        }
        sock->saddr.sin_port = htons(sock->port);
    }
    else
    {
        /* Create UDP socket */
        sock->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock->sockfd == -1)
        {
            printf("udp_init:  Socket create error port %d \n", sock->port);
        }

        /* Determine IP */
        sock->saddr.sin_family = AF_INET;
        if (inet_addr(sock->ip_address) != INADDR_NONE)
        {
            sock->saddr.sin_addr.s_addr = inet_addr(sock->ip_address);
        }
        else
        {
            char ip[16];
            int  check = crypto_host_to_ip(sock->ip_address, ip);
            if (check == 0)
            {
                sock->saddr.sin_addr.s_addr = inet_addr(ip);
            }
        }
        sock->saddr.sin_port = htons(sock->port);
    }

    if (crypto_use_tcp && ((sock->port == TC_APPLY_FWD_PORT || sock->port == TM_PROCESS_PORT)))
    {
        if (bind_sock != 0)
        {
            // TCP server: bind, listen, accept
            if (bind(sock->sockfd, (struct sockaddr *)&sock->saddr, sizeof(sock->saddr)) != 0)
            {
                printf("tcp_init: Bind failed on port %d\n", sock->port);
                return CRYPTO_LIB_ERROR;
            }

            if (listen(sock->sockfd, 1) != 0)
            {
                printf("tcp_init: Listen failed on port %d\n", sock->port);
                return CRYPTO_LIB_ERROR;
            }

            int clientfd = accept(sock->sockfd, NULL, NULL);
            if (clientfd < 0)
            {
                printf("tcp_init: Accept failed on port %d\n", sock->port);
                return CRYPTO_LIB_ERROR;
            }

            // Replace listener with connected client socket
            // close(sock->sockfd); //may be needed
            sock->sockfd = clientfd;
        }
        else
        {
            // TCP client: connect
            if (connect(sock->sockfd, (struct sockaddr *)&sock->saddr, sizeof(sock->saddr)) < 0)
            {
                printf("tcp_init: Connect failed to %s:%d\n", sock->ip_address, sock->port);
                return CRYPTO_LIB_ERROR;
            }
        }
    }
    else
    {
        // UDP: bind only if needed
        if (bind_sock == 0 && sock->port != TM_PROCESS_FWD_PORT && sock->port != TC_APPLY_FWD_PORT)
        {
            status = bind(sock->sockfd, (struct sockaddr *)&sock->saddr, sizeof(sock->saddr));
            if (status != 0)
            {
                perror("bind");

                printf("udp_init: Bind failed on port %d\n", sock->port);
                return CRYPTO_LIB_ERROR;
            }
            // }
        }
        else
        {
            if (crypto_use_tcp == 0 && bind_sock == 1 && sock->port == TM_PROCESS_PORT)
            {
                status = bind(sock->sockfd, (struct sockaddr *)&sock->saddr, sizeof(sock->saddr));
                if (status != 0)
                {
                    perror("bind");

                    printf("udp_init: Bind failed on port %d\n", sock->port);
                    return CRYPTO_LIB_ERROR;
                }
            }
        }
    }

    // Keep-alive socket option (not harmful for UDP, useful for TCP)
    optval = 1;
    optlen = sizeof(optval);
    setsockopt(sock->sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);

    return status;
}

int32_t crypto_reset(void)
{
    int32_t status;

    status = Crypto_Shutdown();
    if (status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
    }

    status = Crypto_SC_Init();
    if (status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
    }

    return status;
}

void crypto_standalone_tc_frame(uint8_t *in_data, uint16_t in_length, uint8_t *out_data, uint16_t *out_length)
{
    /* TC Length */
    if (DYNAMIC_LENGTHS)
    {
        uint8_t segment_hdr_len = 1;
        uint8_t fecf_len        = tc_current_managed_parameters_struct.has_fecf ? 2 : 0;

        *out_length = TC_FRAME_HEADER_SIZE + segment_hdr_len + in_length + fecf_len;
    }
    else
    {
        *out_length = CRYPTO_STANDALONE_FRAMING_TC_DATA_LEN + 6;
    }

    /* TC Header */
    out_data[0] = 0x20;
    out_data[1] = CRYPTO_STANDALONE_FRAMING_SCID;
    out_data[2] = ((tc_vcid << 2) & 0xFC) | (((*out_length - 1) >> 8) & 0x03);
    out_data[3] = (*out_length - 1) & 0xFF;
    out_data[4] = tc_seq_num++;

    /* Segement Header */
    out_data[5] = 0xC0;

    /* SDLS Header */

    /* TC Data */
    memcpy(&out_data[6], in_data, in_length);

    /* SDLS Trailer */
}

void *crypto_standalone_tc_apply(void *socks)
{
    int32_t          status        = CRYPTO_LIB_SUCCESS;
    udp_interface_t *tc_socks      = (udp_interface_t *)socks;
    udp_info_t      *tc_read_sock  = &tc_socks->read;
    udp_info_t      *tc_write_sock = &tc_socks->write;

    uint8_t  tc_apply_in[TC_MAX_FRAME_SIZE];
    uint16_t tc_in_len = 0;
    uint8_t *tc_out_ptr;
    uint16_t tc_out_len = 0;

#ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
    uint8_t tc_framed[TC_MAX_FRAME_SIZE] = {0};
#endif

    int sockaddr_size = sizeof(struct sockaddr_in);

    /* Prepare */
    memset(tc_apply_in, 0x00, sizeof(tc_apply_in));

    while (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        // /* Receive */
        status = recvfrom(tc_read_sock->sockfd, tc_apply_in, sizeof(tc_apply_in), 0,
                          (struct sockaddr *)&tc_read_sock->ip_address, (socklen_t *)&sockaddr_size);
        if (status != -1)
        {
            tc_in_len = status;
            if (tc_debug == 1)
            {
                printf("crypto_standalone_tc_apply - received[%d]: 0x", tc_in_len);
                for (int i = 0; i < status; i++)
                {
                    printf("%02x", tc_apply_in[i]);
                }
                printf("\n");
            }

/* Frame */
#ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
            crypto_standalone_tc_frame(tc_apply_in, tc_in_len, tc_framed, &tc_out_len);
            memcpy(tc_apply_in, tc_framed, tc_out_len);
            tc_in_len  = tc_out_len;
            tc_out_len = 0;
            if (tc_debug == 1)
            {
                printf("crypto_standalone_tc_apply - framed[%d]: 0x", tc_in_len);
                for (int i = 0; i < tc_in_len; i++)
                {
                    printf("%02x", tc_apply_in[i]);
                }
                printf("\n");
            }
#endif

            /* Process */
            status = Crypto_TC_ApplySecurity(tc_apply_in, tc_in_len, &tc_out_ptr, &tc_out_len);
            if (status == CRYPTO_LIB_SUCCESS)
            {
                if (tc_debug == 1)
                {
                    printf("crypto_standalone_tc_apply - status = %d, encrypted[%d]: 0x", status, tc_out_len);
                    for (int i = 0; i < tc_out_len; i++)
                    {
                        printf("%02x", tc_out_ptr[i]);
                    }
                    printf("\n");
                }
                // printf("About to write to port %d!\n", tc_write_sock->port);
                /* Reply */
                if (crypto_use_tcp)
                {
                    status = send(tc_write_sock->sockfd, tc_out_ptr, tc_out_len, 0);
                }
                else
                {
                    status = sendto(tc_write_sock->sockfd, tc_out_ptr, tc_out_len, 0,
                                    (struct sockaddr *)&tc_write_sock->saddr, sizeof(tc_write_sock->saddr));
                }
                if ((status == -1) || (status != tc_out_len))
                {
                    printf("crypto_standalone_tc_apply - Reply error %d \n", status);
                }
                // printf("Allegedly wrote %d bytes to port %d!\n", tc_out_len, tc_write_sock->port);
            }
            else
            {
                printf("crypto_standalone_tc_apply - ApplySecurity error %d \n", status);
            }

            /* Reset */
            memset(tc_apply_in, 0x00, sizeof(tc_apply_in));
            memset(tc_framed, 0x00, sizeof(tc_framed));
            tc_in_len  = 0;
            tc_out_len = 0;
            if (!tc_out_ptr)
                free(tc_out_ptr);
            if (tc_debug == 1)
            {
#ifdef CRYPTO_STANDALONE_TC_APPLY_DEBUG
                printf("\n");
#endif
            }
        }

        /* Delay */
        usleep(100);
    }
    close(tc_read_sock->sockfd);
    close(tc_write_sock->sockfd);
    return tc_read_sock;
}

void crypto_standalone_tm_frame(uint8_t *in_data, uint16_t in_length, uint8_t *out_data, uint16_t *out_length,
                                uint16_t spi)
{
    SaInterface            sa_if  = get_sa_interface_inmemory();
    SecurityAssociation_t *sa_ptr = NULL;
    int32_t                status = CRYPTO_LIB_SUCCESS;

    status = sa_if->sa_get_from_spi(spi, &sa_ptr);
    if (status != CRYPTO_LIB_SUCCESS)
    {
        printf("WARNING - SA IS NULL!\n");
    }

    // Calculate security headers and trailers
    uint8_t header_length =
        TM_PRI_HDR_LENGTH + SDLS_SPI_LENGTH + sa_ptr->shivf_len + sa_ptr->shplf_len + sa_ptr->shsnf_len;

    uint8_t trailer_length = sa_ptr->stmacf_len;
    if (tm_current_managed_parameters_struct.has_fecf == TM_HAS_FECF)
    {
        trailer_length += 2;
    }
    if (tm_current_managed_parameters_struct.has_ocf == TM_HAS_OCF)
    {
        trailer_length += 4;
    }

    /* TM Length */
    *out_length = (uint16_t)in_length - header_length - trailer_length;

    /* TM Header */
    memcpy(out_data, &in_data[header_length], in_length - header_length - trailer_length);
}

void crypto_standalone_tm_debug_recv(int32_t status, int tm_process_len, uint8_t *tm_process_in)
{
    if (tm_debug == 1)
    {
        printf("crypto_standalone_tm_process - received[%d]: 0x", tm_process_len);
        for (int i = 0; i < status; i++)
        {
            printf("%02x", tm_process_in[i]);
        }
        printf("\n");
    }
}

void crypto_standalone_tm_debug_process(uint8_t *tm_process_in)
{
    if (tm_debug == 1)
    {
        printf("Printing first bytes of Tf Pri Hdr:\n\t");
        for (int i = 0; i < 6; i++)
        {
            printf("%02X", *(tm_process_in + 4 + i));
        }
        printf("\n");
        printf("Processing frame WITH ASM...\n");
    }
}

void crypto_standalone_spp_telem_or_idle(int32_t *status, uint8_t *tm_ptr, uint16_t *spp_len, udp_interface_t *tm_socks,
                                         int *tm_process_len)
{
    udp_info_t *tm_write_sock = &tm_socks->write;

    if ((tm_ptr[0] == 0x08) || (tm_ptr[0] == 0x09) || ((tm_ptr[0] == 0x07) && (tm_ptr[1] == 0xff)) ||
        (tm_ptr[0] == 0x0F && tm_ptr[1] == 0xFD))
    {
        *spp_len = (((0xFFFF & tm_ptr[4]) << 8) | tm_ptr[5]) + 7;
#ifdef CRYPTO_STANDALONE_TM_PROCESS_DEBUG
        printf("crypto_standalone_tm_process - SPP[%d]: 0x", *spp_len);
        for (int i = 0; i < *spp_len; i++)
        {
            printf("%02x", tm_ptr[i]);
        }
        printf("\n");
#endif
        // Send all SPP telemetry packets
        // 0x09 for HK/Device TLM Packets (Generic Components)
        // 0x0FFD = CFDP
        if (tm_ptr[0] == 0x08 || tm_ptr[0] == 0x09 || (tm_ptr[0] == 0x0f && tm_ptr[1] == 0xfd))
        {
            *status = sendto(tm_write_sock->sockfd, tm_ptr, *spp_len, 0, (struct sockaddr *)&tm_write_sock->saddr,
                             sizeof(tm_write_sock->saddr));
        }
        // Only send idle packets if configured to do so
        else
        {
#ifdef CRYPTO_STANDALONE_DISCARD_IDLE_PACKETS
            // Don't forward idle packets
            *status = *spp_len;
#else
            *status = sendto(tm_write_sock->sockfd, tm_ptr, *spp_len, 0, (struct sockaddr *)&tm_write_sock->saddr,
                             sizeof(tm_write_sock->saddr));
#endif
        }

        // Check status
        if ((*status == -1) || (*status != *spp_len))
        {
            printf("crypto_standalone_tm_process - Reply error %d \n", *status);
        }

        *tm_process_len -= *spp_len;
    }
    else if ((tm_ptr[0] == 0xFF && tm_ptr[1] == 0x48) || (tm_ptr[0] == 0x00 && tm_ptr[1] == 0x00) ||
             (tm_ptr[0] == 0x02 && tm_ptr[1] == 0x00) || (tm_ptr[0] == 0xFF && tm_ptr[1] == 0xFF))
    {
        // TODO: Why 0x0200?
        // Idle Frame
        // Idle Frame is entire length of remaining data
#ifdef CRYPTO_STANDALONE_DISCARD_IDLE_FRAMES
        // Don't forward idle frame
        *status = *spp_len;
#else
        *status = sendto(tm_write_sock->sockfd, tm_ptr, *spp_len, 0, (struct sockaddr *)&tm_write_sock->saddr,
                         sizeof(tm_write_sock->saddr));
        if ((*status == -1) || (*status != *spp_len))
        {
            printf("crypto_standalone_tm_process - Reply error %d \n", *status);
        }
#endif
        *tm_process_len = 0;
    }
    else
    {
        printf("crypto_standalone_tm_process - SPP loop error, expected idle packet or frame! tm_ptr = 0x%02x%02x \n",
               tm_ptr[0], tm_ptr[1]);
        *tm_process_len = 0;
    }
}

void *crypto_standalone_tm_process(void *socks)
{
    int32_t          status        = CRYPTO_LIB_SUCCESS;
    udp_interface_t *tm_socks      = (udp_interface_t *)socks;
    udp_info_t      *tm_read_sock  = &tm_socks->read;
    udp_info_t      *tm_write_sock = &tm_socks->write;

    uint8_t  tm_process_in[TM_CADU_SIZE]; // Accounts for ASM automatically based on #def
    int      tm_process_len = 0;
    uint16_t spp_len        = 0;
    uint8_t *tm_ptr;
    uint16_t tm_out_len = 0;

#ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
    uint8_t  tm_framed[TM_CADU_SIZE];
    uint16_t tm_framed_len = 0;
#endif

    int sockaddr_size = sizeof(struct sockaddr_in);

    while (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        /* Receive */
        if (crypto_use_tcp)
        {
            status = recv(tm_read_sock->sockfd, tm_process_in, sizeof(tm_process_in), 0);
            if (status == -1)
            {
                printf(" Problem with recv TCP tm_proccess: status = %d \n", status);
            }
        }
        else
        {
            status = recvfrom(tm_read_sock->sockfd, tm_process_in, sizeof(tm_process_in), 0,
                              (struct sockaddr *)&tm_read_sock->ip_address, (socklen_t *)&sockaddr_size);
        }
        if (status != -1)
        {
            tm_process_len = status;
            /* Receive */
            crypto_standalone_tm_debug_recv(status, tm_process_len, tm_process_in);
            /* Process */
#ifdef TM_CADU_HAS_ASM
            // Process Security skipping prepended ASM
            crypto_standalone_tm_debug_process(tm_process_in);
            // Account for ASM length
            if (tm_process_in[4] == 0x40)
            {
                status = Crypto_AOS_ProcessSecurity(tm_process_in + 4, (const uint16_t)tm_process_len - 4, &tm_ptr,
                                                    &tm_out_len);
                if (status != 0)
                {
                    printf("Crypto_AOS_ProcessSecurity Failed with status = %d\n", status);
                }
            }
            else
            {
                status = Crypto_TM_ProcessSecurity(tm_process_in + 4, (const uint16_t)tm_process_len - 4, &tm_ptr,
                                                   &tm_out_len);
                if (status != 0)
                {
                    printf("Crypto_TM_ProcessSecurity Failed with status = %d\n", status);
                }
            }
#else
            if (tm_debug == 1)
            {
                printf("Processing frame without ASM...\n");
            }
            status = Crypto_TM_ProcessSecurity(tm_process_in, (const uint16_t)tm_process_len, &tm_ptr, &tm_out_len);
            if (status != 0)
            {
                printf("Crypto_TM_ProcessSecurity Failed with status = %d\n", status);
            }
#endif
            if (status == CRYPTO_LIB_SUCCESS)
            {
                if (tm_debug == 1)
                {
                    if ((tm_ptr[4] == 0x07) && (tm_ptr[5] == 0xFF))
                    {
                        // OID Frame
                    }
                    else
                    {
                        printf("crypto_standalone_tm_process: 1 - status = %d, decrypted[%d]: 0x", status, tm_out_len);
                        for (int i = 0; i < tm_out_len; i++)
                        {
                            printf("%02x", tm_ptr[i]);
                        }
                        printf("\n");
                    }
                }

/* Frame */
#ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
#ifdef TM_CADU_HAS_ASM
                uint16_t spi = (tm_process_in[10] << 8) | tm_process_in[11];
                crypto_standalone_tm_frame(tm_ptr, tm_out_len, tm_framed, &tm_framed_len, spi);
#else
                uint16_t spi = (tm_process_in[6] << 8) | tm_process_in[7];
                crypto_standalone_tm_frame(tm_process_in, tm_process_len, tm_framed, &tm_framed_len, spi);
#endif
                memcpy(tm_process_in, tm_framed, tm_framed_len);
                tm_process_len = tm_framed_len;
                tm_framed_len  = 0;

                if (tm_debug == 1)
                // Note: Need logic to allow broken packet assembly
                {
                    printf("crypto_standalone_tm_process: 2 - beginning after first header pointer - deframed[%d]: 0x",
                           tm_process_len);
                    for (int i = 0; i < tm_process_len; i++)
                    {
                        printf("%02x", tm_framed[i]);
                    }
                    printf("\n");
                }
#endif

                /* Space Packet Protocol Loop */
                tm_ptr = &tm_process_in[0];
                while (tm_process_len > 5)
                {
                    // SPP Telemetry OR SPP Idle Packet
                    crypto_standalone_spp_telem_or_idle(&status, tm_ptr, &spp_len, tm_socks, &tm_process_len);
                    tm_ptr = &tm_ptr[spp_len];
                }
            }
            else
            {
                printf("crypto_standalone_tm_process - ProcessSecurity error %d \n", status);
            }

            /* Reset */
            memset(tm_process_in, 0x00, sizeof(tm_process_in));
            tm_process_len = 0;
            memset(tm_ptr, 0x00, sizeof(tm_process_in));
#ifdef CRYPTO_STANDALONE_TM_PROCESS_DEBUG
            printf("\n");
#endif
        }

        /* Delay */
        usleep(10);
    }
    close(tm_read_sock->sockfd);
    close(tm_write_sock->sockfd);
    return tm_read_sock;
}

void crypto_standalone_cleanup(const int signal)
{
    if (signal == SIGINT)
    {
        printf("\n");
        printf("Received CTRL+C, cleaning up... \n");
    }
    /* Signal threads to stop */
    keepRunning = CRYPTO_LIB_ERROR;
    exit(signal);
    return;
}

int main(int argc, char *argv[])
{
    int32_t status = CRYPTO_LIB_SUCCESS;

    char  input_buf[CRYPTO_MAX_INPUT_BUF];
    char  input_tokens[CRYPTO_MAX_INPUT_TOKENS][CRYPTO_MAX_INPUT_TOKEN_SIZE];
    int   num_input_tokens;
    int   cmd;
    char *token_ptr;

    udp_interface_t tc_apply;
    udp_interface_t tm_process;

    pthread_t tc_apply_thread;
    pthread_t tm_process_thread;

    tc_apply.read.ip_address    = CRYPTOLIB_HOSTNAME;
    tc_apply.read.port          = TC_APPLY_PORT;
    tc_apply.write.ip_address   = SC_HOSTNAME;
    tc_apply.write.port         = TC_APPLY_FWD_PORT;
    tm_process.read.ip_address  = CRYPTOLIB_HOSTNAME;
    tm_process.read.port        = TM_PROCESS_PORT;
    tm_process.write.ip_address = GSW_HOSTNAME;
    tm_process.write.port       = TM_PROCESS_FWD_PORT;

    printf("Starting CryptoLib in standalone mode! \n");
    if (argc != 1)
    {
        printf("Invalid number of arguments! \n");
        printf("  Expected zero but received: %s \n", argv[1]);
    }
    printf("CryptoLib using %s sockets\n", crypto_use_tcp ? "TCP" : "UDP");

    /* Catch CTRL+C */
    signal(SIGINT, crypto_standalone_cleanup);

    /* Startup delay */
    sleep(10);

    /* Initialize CryptoLib */
    status = crypto_reset();
    if (status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
        keepRunning = CRYPTO_LIB_ERROR;
    }

    /* Initialize sockets */
    if (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        status = crypto_standalone_socket_init(&tc_apply.read, TC_APPLY_PORT, 0, 0); // udp 6010
        if (status != CRYPTO_LIB_SUCCESS)
        {
            printf("crypto_standalone_socket_init tc_apply.read failed with status %d \n", status);
            keepRunning = CRYPTO_LIB_ERROR;
        }
        else
        {
            status = crypto_standalone_socket_init(&tc_apply.write, TC_APPLY_FWD_PORT, 0,
                                                   crypto_use_tcp); // tcp, connect() 8010
            if (status != CRYPTO_LIB_SUCCESS)
            {
                printf("crypto_standalone_socket_init tc_apply.write failed with status %d \n", status);
                keepRunning = CRYPTO_LIB_ERROR;
            }
        }
    }

    if (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        status =
            crypto_standalone_socket_init(&tm_process.read, TM_PROCESS_PORT, 1, crypto_use_tcp); // tcp, accept() 8011
        if (status != CRYPTO_LIB_SUCCESS)
        {
            printf("crypto_standalone_socket_init tm_apply.read failed with status %d \n", status);
            keepRunning = CRYPTO_LIB_ERROR;
        }
        else
        {
            status = crypto_standalone_socket_init(&tm_process.write, TM_PROCESS_FWD_PORT, 0, 0); // udp 6011
            if (status != CRYPTO_LIB_SUCCESS)
            {
                printf("crypto_standalone_socket_init tm_process.write failed with status %d \n", status);
                keepRunning = CRYPTO_LIB_ERROR;
            }
        }
    }

    /* Start threads */
    if (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        printf("  TC Apply \n");
        printf("    Read, UDP - %s : %d \n", tc_apply.read.ip_address, tc_apply.read.port);
        printf("    Write, %s - %s : %d \n", crypto_use_tcp ? "TCP" : "UDP", tc_apply.write.ip_address,
               tc_apply.write.port);
        printf("  TM Process \n");
        printf("    Read, %s - %s : %d \n", crypto_use_tcp ? "TCP" : "UDP", tm_process.read.ip_address,
               tm_process.read.port);
        printf("    Write, UDP - %s : %d \n", tm_process.write.ip_address, tm_process.write.port);
        printf("\n");

        status = pthread_create(&tc_apply_thread, NULL, *crypto_standalone_tc_apply, &tc_apply);
        if (status < 0)
        {
            perror("Failed to create tc_apply_thread thread");
            keepRunning = CRYPTO_LIB_ERROR;
        }
        else
        {
            status = pthread_create(&tm_process_thread, NULL, *crypto_standalone_tm_process, &tm_process);
            if (status < 0)
            {
                perror("Failed to create tm_process_thread thread");
                keepRunning = CRYPTO_LIB_ERROR;
            }
        }
    }

    /* Main loop */
    while (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        num_input_tokens = -1;
        cmd              = CRYPTO_CMD_UNKNOWN;

        /* Read user input */
        printf(CRYPTO_PROMPT);
        fgets(input_buf, CRYPTO_MAX_INPUT_BUF, stdin);

        /* Tokenize line buffer */
        token_ptr = strtok(input_buf, " \t\n");
        while ((num_input_tokens < CRYPTO_MAX_INPUT_TOKENS) && (token_ptr != NULL))
        {
            if (num_input_tokens == -1)
            {
                /* First token is command */
                cmd = crypto_standalone_get_command(token_ptr);
            }
            else
            {
                strncpy(input_tokens[num_input_tokens], token_ptr, CRYPTO_MAX_INPUT_TOKEN_SIZE);
            }
            token_ptr = strtok(NULL, " \t\n");
            num_input_tokens++;
        }

        /* Process command if valid */
        if (num_input_tokens >= 0)
        {
            crypto_standalone_process_command(cmd, num_input_tokens, &input_tokens[0][0]);
        }
    }

    /* Cleanup */
    close(tc_apply.read.sockfd);
    close(tc_apply.write.sockfd);
    close(tm_process.read.sockfd);
    close(tm_process.write.sockfd);

    Crypto_Shutdown();

    printf("\n");
    exit(status);
}
