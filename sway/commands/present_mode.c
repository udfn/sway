#include <strings.h>
#include "log.h"
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/view.h"
#include "sway/tree/workspace.h"
#include "util.h"


static enum wlr_output_present_mode string_to_present_mode(char *str) {
	if (strncmp("normal", str, 4) == 0) {
		return WLR_OUTPUT_PRESENT_MODE_NORMAL;
	} else if (strncmp("adaptive", str, 4) == 0) {
		return WLR_OUTPUT_PRESENT_MODE_ADAPTIVE;
	} else if (strncmp("immediate",str,4) == 0) {
		return WLR_OUTPUT_PRESENT_MODE_IMMEDIATE;
	}
	sway_log(SWAY_ERROR,"Unknown present mode %s", str);
	return WLR_OUTPUT_PRESENT_MODE_NORMAL;
}

// present_mode [immediate|adaptive|normal]
struct cmd_results *cmd_present_mode(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "present_mode", EXPECTED_AT_MOST, 1))) {
		return error;
	}
	if (!root->outputs->length) {
		return cmd_results_new(CMD_FAILURE,
				"Can't run this command while there's no outputs connected.");
	}
	struct sway_node *node = config->handler_context.node;
	struct sway_container *container = config->handler_context.container;
	struct sway_workspace *workspace = config->handler_context.workspace;
	if (node->type == N_WORKSPACE && workspace->tiling->length == 0) {
		return cmd_results_new(CMD_FAILURE,
				"Can't change present mode on an empty workspace.");
	}

	// If in the scratchpad, operate on the highest container
	if (container && !container->pending.workspace) {
		while (container->pending.parent) {
			container = container->pending.parent;
		}
	}
	enum wlr_output_present_mode desired_mode = string_to_present_mode(argv[0]);
	for (struct sway_container *curr = container; curr; curr = curr->pending.parent) {
		if (curr->fullscreen_present_mode) {
			container = curr;
			break;
		}
	}
	container->fullscreen_present_mode = container->fullscreen_present_mode == desired_mode ? WLR_OUTPUT_PRESENT_MODE_NORMAL : desired_mode;
	return cmd_results_new(CMD_SUCCESS, NULL);
}
