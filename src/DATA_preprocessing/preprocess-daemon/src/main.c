/**
 * @file main.c
 * @author Frederic Simard (frederic.simard.1@outlook.com) | Atlants Embedded
 * @brief 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "data_structure.h"
#include "app_signal.h"
#include "data_input.h"
#include "feature_output.h"
#include "xml.h"
#include "preprocess_core.h"

#define CONFIG_NAME "config/preprocess_config.xml"

static data_t data_input;
static data_t feature_output;

void init_app_buffers(appconfig_t *config);
void cleanup_app_buffers(appconfig_t *config);

/**
 * main()
 * @brief Application main running loop
 */
int main(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused)))
{
	int i,j;
	
	/*Set up ctrl c signal handler*/
	(void)signal(SIGINT, ctrl_c_handler);

	/*read the config from the xml*/
	appconfig_t *config = (appconfig_t *) xml_initialize(CONFIG_NAME);
	if (config == NULL) {
		printf("Error initializing XML configuration\n");
		return (-1);
	}
	printf("xml configuration read\n");
	
	/*Initialize data input*/
	if(init_data_input(config->data_source)){
		printf("Error initializing data input");
		return (-1);
	}
	printf("Data input initialized\n");
	
	/*Initialize feature output*/
	if(init_feature_output(config->feature_dest)){
		printf("Error initializing feature output");
		return (-1);
	}
	printf("Feature output initialized\n");

	init_app_buffers(config);
	init_preprocess_core(config);
	
	/*Begin data processing*/
	for(i=0;i<10;i++)
	{
		/*read data*/
		READ_DATA_FC(&data_input);
		
		/*preprocess into a feature vector*/
		preprocess_data(&data_input, &feature_output);
		
		/*write feature vector*/
		WRITE_DATA_FC(&feature_output);
	}

	TERMINATE_DATA_INPUT_FC();
	TERMINATE_FEATURE_OUTPUT_FC();
	
	cleanup_preprocess_core(config);
	cleanup_app_buffers(config);
	
	return 0;
}

void init_app_buffers(appconfig_t *config){
	
	data_input.type = DOUBLE;
	data_input.nb_data = config->nb_channels*config->buffer_depth;
	data_input.ptr = malloc(config->nb_channels*config->buffer_depth*sizeof(double));
	
	feature_output.type = DOUBLE;
	feature_output.nb_data = config->nb_channels;
	feature_output.ptr = malloc(config->nb_channels*sizeof(double));
}
	
void cleanup_app_buffers(appconfig_t *config){
	
	free(data_input.ptr);
	free(feature_output.ptr);
	
}
