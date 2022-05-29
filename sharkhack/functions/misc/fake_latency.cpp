#include "fake_latency.h"

std::deque<incoming_sequence> sequences;
int32_t last_incoming_sequence_number;

void fake_latency::add_latency_to_net_channel(INetChannel* net_channel, float latency) {

	if (!g_engine->IsInGame() || !g_engine->IsConnected())
		return;

	for (auto& seq : sequences) {
		if (g_globals->realtime - seq.cur_time >= latency) {
			net_channel->m_nInReliableState = seq.in_reliable_state;
			net_channel->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}

void fake_latency::update_incoming_sequences() {

	if (!g_client_state)
		return;

	INetChannel* net_channel = reinterpret_cast<INetChannel*>(g_client_state->m_NetChannel);

	if (!net_channel)
		return;

	if (net_channel->m_nInSequenceNr > last_incoming_sequence_number) {
		last_incoming_sequence_number = net_channel->m_nInSequenceNr;
		sequences.push_front(incoming_sequence(net_channel->m_nInReliableState, net_channel->m_nOutReliableState, net_channel->m_nInSequenceNr, g_globals->realtime));
	}

	if (sequences.size() > 2048)
		sequences.pop_back();
}