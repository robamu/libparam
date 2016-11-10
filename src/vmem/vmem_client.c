/*
 * vmem_client.c
 *
 *  Created on: Oct 27, 2016
 *      Author: johan
 */

#include <stdio.h>
#include <csp/arch/csp_malloc.h>
#include <csp/csp_endian.h>

#include <vmem/vmem_server.h>
#include <vmem/vmem_client.h>

void vmem_download(int node, int timeout, uint32_t address, uint32_t length, char * dataout)
{
	/* Establish RDP connection */
	csp_conn_t * conn = csp_connect(CSP_PRIO_HIGH, node, VMEM_PORT_SERVER, timeout, CSP_O_RDP);
	if (conn == NULL)
		return;

	csp_packet_t * packet = csp_buffer_get(sizeof(vmem_request_t));
	vmem_request_t * request = (void *) packet->data;
	request->version = 1;
	request->type = VMEM_SERVER_DOWNLOAD;
	request->address = csp_hton32(address);
	request->length = csp_hton32(length);
	packet->length = sizeof(vmem_request_t);

	/* Send request */
	if (!csp_send(conn, packet, timeout)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return;
	}

	/* Go into download loop */
	int count = 0;
	int dotcount = 0;
	while((packet = csp_read(conn, timeout)) != NULL) {

		//csp_hex_dump("Download", packet->data, packet->length);

		printf(".");
		fflush(stdout);
		dotcount++;
		if (dotcount % 32 == 0)
			printf(" - %u\n", count);

		/* Put data */
		memcpy((void *) ((intptr_t) dataout + count), packet->data, packet->length);

		/* Increment */
		count += packet->length;

		csp_buffer_free(packet);
	}

	printf(" - %u\n", count);

	csp_close(conn);
}

void vmem_upload(int node, int timeout, uint32_t address, char * datain, uint32_t length)
{
	/* Establish RDP connection */
	csp_conn_t * conn = csp_connect(CSP_PRIO_HIGH, node, VMEM_PORT_SERVER, timeout, CSP_O_RDP);
	if (conn == NULL)
		return;

	csp_packet_t * packet = csp_buffer_get(sizeof(vmem_request_t));
	vmem_request_t * request = (void *) packet->data;
	request->version = 1;
	request->type = VMEM_SERVER_UPLOAD;
	request->address = csp_hton32(address);
	request->length = csp_hton32(length);
	packet->length = sizeof(vmem_request_t);

	/* Send request */
	if (!csp_send(conn, packet, timeout)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return;
	}

	int count = 0;
	int dotcount = 0;
	while(count < length) {

		printf(".");
		fflush(stdout);
		dotcount++;
		if (dotcount % 32 == 0)
			printf(" - %u\n", count);

		/* Prepare packet */
		csp_packet_t * packet = csp_buffer_get(VMEM_SERVER_MTU);
		packet->length = VMEM_MIN(VMEM_SERVER_MTU, length - count);

		/* Copy data */
		memcpy(packet->data, (void *) ((intptr_t) datain + count), packet->length);

		/* Increment */
		count += packet->length;

		if (!csp_send(conn, packet, VMEM_SERVER_TIMEOUT)) {
			csp_buffer_free(packet);
			return;
		}

	}

	printf(" - %u\n", count);

	csp_close(conn);

}