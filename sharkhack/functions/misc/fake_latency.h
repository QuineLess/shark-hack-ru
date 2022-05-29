#pragma once

#include "../../utils/entity.h"
#include <deque>

struct incoming_sequence {
	incoming_sequence(int in_state, int out_state, int seqnr, float time) {
		in_reliable_state = in_state;
		out_reliable_state = out_state;
		sequencenr = seqnr;
		cur_time = time;
	}
	int in_reliable_state;
	int out_reliable_state;
	int sequencenr;
	float cur_time;
};

namespace fake_latency {
	void add_latency_to_net_channel(INetChannel * net_channel, float latency);
	void update_incoming_sequences();
};