#include <strings.h>
#include "log.h"
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/view.h"
#include "sway/tree/workspace.h"
#include "util.h"

// always_on_top [enable|disable|toggle]
struct cmd_results *cmd_always_on_top(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "always_on_top", EXPECTED_AT_MOST, 1))) {
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
				"An empty workspace can't be always on top.");
	}

	// If in the scratchpad, operate on the highest container
	if (container && !container->pending.workspace) {
		while (container->pending.parent) {
			container = container->pending.parent;
		}
	}

	bool is_always_on_top = false;
	for (struct sway_container *curr = container; curr; curr = curr->pending.parent) {
		if (curr->always_on_top) {
			container = curr;
			is_always_on_top = true;
			break;
		}
	}
	bool enable = !is_always_on_top;
	if (argc > 0) {
		enable = parse_boolean(argv[0], is_always_on_top);
	}
	container->always_on_top = enable;
	return cmd_results_new(CMD_SUCCESS, NULL);
}
