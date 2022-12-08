#include "static_rules_table.h"

rule_t static_rules_table[MAX_RULES];
unsigned int number_of_rules_in_table = 0;

void prepare_static_rules_table(void) {
	printk(KERN_INFO "prepare the firewall static rules table\n");
}

void add_static_table_rule(const char *buf, size_t count) {
	if(number_of_rules_in_table >= 50) {
		printk(KERN_INFO "Tried insert another rule to a full static rules table\n");
		return;
	}

	strncpy(static_rules_table[number_of_rules_in_table].rule_name = strtok(buf, "-"), 20);
 	static_rules_table[number_of_rules_in_table].direction = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].src_ip = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].src_prefix_size = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].dst_ip = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].dst_prefix_size = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].src_port = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].dst_port = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].protocol = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].ack = strtok(NULL, "-");
	static_rules_table[number_of_rules_in_table].action = strtok(NULL, "-");

	number_of_rules_in_table++;
}

void delete_static_rule_table() {
	memset(static_rules_table, 0 , sizeof(static_rules_table));
	number_of_rules_in_table = 0;
}

int check_packet_against_rules_table(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
	int i;
	int is_rule_match;

	if(is_cristmas_tree_packet(skb)) {
		return NF_DROP;
	}

	if(is_irrelevant_packet(skb)) {
		return NF_ACCEPT;
	}

	for(i = 0; i < MAX_RULES; i++) {
		is_rule_match = check_rule_for_match(&static_rules_table[i], struct sk_buff *skb);
		if(is_rule_match == RULE_MATCHES) {
			//TODO add this to the log
			return static_rules_table[i].action;
		}
	}
	//TODO add this to the log
	return NF_DROP;
}

int check_rule_for_match(rule_t *table_entry_ptr, struct sk_buff *skb) {
	struct tcphdr *tcp_header = tcp_hdr(skb);
	struct iphdr * ip_header = ip_hdr(skb);
	
	//TODO: check the direction field

	// check if saddr is in the netowrk src_ip/src_prefix_size
	if(table_entry_ptr->src_ip != 0 &&
	  (ip_header->saddr & table_entry_ptr->src_prefix_mask) != (table_entry_ptr->src_ip & table_entry_ptr->src_prefix_mask) {
		return RULE_DOESNOT_MATCHES;
	}

	// check if daddr is in the netowrk dst_ip/dst_prefix_size
	if(table_entry_ptr->dst_ip != 0 &&
	  (ip_header->daddr & table_entry_ptr->dst_prefix_mask) != (table_entry_ptr->dst_ip & table_entry_ptr->dst_prefix_mask)) {
		return RULE_DOESNOT_MATCHES;
	}

	if(table_entry_ptr->protocol != prot_t.PROT_ANY && ip_header->protocol != table_entry_ptr->protocol) {
		return RULE_DOESNOT_MATCHES;
	}

	if(table_entry_ptr->src_port != 0 && tcp_header->source != table_entry_ptr->src_port) {
		return RULE_DOESNOT_MATCHES;
	}

	if(table_entry_ptr->dst_port != 0 && tcp_header->dest != table_entry_ptr->dst_port) {
		return RULE_DOESNOT_MATCHES;
	}

	if(table_entry_ptr->ack != ack_t.ACK_ANY && tcp_header->ack != table_entry_ptr->ack) {
		return RULE_DOESNOT_MATCHES;
	}

	return RULE_MATCHES;
}

// true if all flags are set:
// Terminate the connection flag, Urgent flag and Push data immediately flag
int is_cristmas_tree_packet(struct sk_buff *skb) {
	struct tcphdr *tcp_header = tcp_hdr(skb);
	return (tcp_header->fin == 1 && tcp_header->urg == 1 && tcp_header->psh == 1);
}

// true if ip protocol not in {tcp, udp, icmp} or
// ipv6 packet
int is_irrelevant_packet(struct sk_buff *skb) {
	struct iphdr * ip_header = ip_hdr(skb);
	int is_irrelevant_protocol = (ip_header->protocol != IPPROTO_TCP &&
		ip_header->protocol != IPPROTO_UDP &&
		ip_header->protocol != IPPROTO_ICMP);
	//TODO: fill in the missing data 
	int is_loopback_packet = 0;

	return is_irrelevant_protocol || (ip_header->version == 6) || is_loopback_packet;
}