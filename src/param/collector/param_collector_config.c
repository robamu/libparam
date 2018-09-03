/*
 * param_collector_config.c
 *
 *  Created on: Aug 30, 2018
 *      Author: johan
 */

#include <stdio.h>
#include <string.h>
#include <param/param.h>
#include <param_config.h>

#include "param_collector_config.h"

struct param_collector_config_s param_collector_config[16] = {0};

void param_col_confstr_callback(struct param_s * param, int offset) {
	printf("Callback\n");
	param_collector_init();
}

extern const vmem_t vmem_col;
PARAM_DEFINE_STATIC_VMEM(PARAMID_COLLECTOR_CNFSTR, col_cnfstr, PARAM_TYPE_STRING, 100, 0,  PM_CONF, param_col_confstr_callback, "", col, 0x00, NULL);

void param_collector_init(void) {
	char buf[100];
	param_get_data(&col_cnfstr, buf, 100);
	int len = strnlen(buf, 100);
	printf("Init with str: %s, len %u\n", buf, len);

	/* Get first token */
	char *saveptr;
	char * str = strtok_r(buf, ",", &saveptr);
	int i = 0;

	while ((str) && (strlen(str) > 1)) {
		unsigned int node, interval, mask = 0xFFFFFFFF;
		if (sscanf(str, "%u %u %x", &node, &interval, &mask) != 3) {
			if (sscanf(str, "%u %u", &node, &interval) != 2) {
				printf("Parse error %s", str);
				return;
			}
		}
		printf("Parsed %u %u %x\n", node, interval, mask);

		param_collector_config[i].node = node;
		param_collector_config[i].interval = interval;
		param_collector_config[i].mask = mask;

		i++;
		str = strtok_r(NULL, ",", &saveptr);
	}

}
