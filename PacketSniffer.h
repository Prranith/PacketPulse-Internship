#ifndef PACKET_SNIFFER_H
#define PACKET_SNIFFER_H

#include <pcap.h>
#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <utility>

#pragma comment(lib, "ws2_32.lib")

// Ethernet header
struct ethernet_header {
    u_char dest_mac[6];
    u_char src_mac[6];
    u_short ether_type;
};

// IPv4 header
struct ip_header {
    u_char ver_ihl;        // Version (4 bits) + Header length (4 bits)
    u_char tos;            // Type of service
    u_short tlen;          // Total length
    u_short identification; // Identification
    u_short flags_fo;      // Flags (3 bits) + Fragment offset (13 bits)
    u_char ttl;            // Time to live
    u_char proto;          // Protocol
    u_short crc;           // Header checksum
    in_addr src_addr;      // Source address
    in_addr dst_addr;      // Destination address
};

// TCP header
struct tcp_header {
    u_short src_port;
    u_short dst_port;
    u_int seq;
    u_int ack_seq;
    u_char offset_res;
    u_char flags;
    u_short window;
    u_short checksum;
    u_short urgent_ptr;
};

// UDP header
struct udp_header {
    u_short src_port;
    u_short dst_port;
    u_short length;
    u_short checksum;
};

// ICMP header
struct icmp_header {
    u_char type;
    u_char code;
    u_short checksum;
    u_short id;
    u_short seq;
};

// Packet metadata structure (enhanced for all OSI layers)
struct PacketMetadata {
    // Layer 1: Physical
    std::string interface_name;
    std::string interface_description;

    // Layer 2: Data Link
    std::string src_mac;
    std::string dst_mac;
    std::string eth_type;
    std::string vlan_tag;

    // Layer 3: Network
    std::string src_ip;
    std::string dst_ip;
    std::string protocol;
    std::string ip_options;
    std::string direction; // "Incoming" or "Outgoing"

    // Layer 4: Transport
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    std::string tcp_flags;
    std::string transport_options;
    std::string checksum_status;

    // Layer 5: Session
    std::string session_id;
    std::string session_state;

    // Layer 6: Presentation
    std::string tls_version;
    std::string cipher_suite;
    std::string compression;

    // Layer 7: Application
    std::string app_protocol;
    std::string app_data;
    std::string dns_query;
    std::string http_info;
    std::string smtp_info;
    std::string imap_info;
    std::string pop3_info;
    std::string sip_info;
    std::string rtp_info;
    std::string ftp_info;
    std::string icmp_info;

    // General packet info
    std::string timestamp;
    double inter_arrival = 0.0;
    uint32_t captured_length = 0;
    uint32_t total_length = 0;
    uint32_t payload_size = 0;
    std::string payload_hex_ascii;
};

// Flow statistics structure
struct FlowStats {
    int packet_count = 0;
    uint64_t total_bytes = 0;
    double last_timestamp = 0;
    double byte_rate = 0;
};

#define ETHER_TYPE_IP 0x0800
#define ETHER_TYPE_IPV6 0x86DD
#define ETHER_TYPE_VLAN 0x8100
#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17
#define IP_PROTO_ICMP 1

class PacketSniffer {
public:
    PacketSniffer(const std::string& deviceName = "");
    ~PacketSniffer();
    void listInterfaces();
    void startSniffing(int interfaceIndex);

private:
    static void packetHandler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);
    std::string macToString(const u_char* mac);
    std::string ipToString(const in_addr& addr);
    std::string ipv6ToString(const u_char* addr);
    std::string getTcpFlags(const tcp_header* tcp_hdr);
    void printPacketDetails(const struct pcap_pkthdr* header, const u_char* pkt_data);
    void logPayload(const u_char* payload, int payload_size, PacketMetadata& meta);
    std::string parseIpOptions(const u_char* options, int options_len);
    std::string parseTcpOptions(const u_char* options, int options_len);
    std::string parseApplicationPayload(const u_char* payload, int payload_size, const std::string& protocol, uint16_t src_port, uint16_t dst_port, PacketMetadata& meta);
    std::string parseDnsQuery(const u_char* payload, int payload_size);
    std::string parseTlsVersion(const u_char* payload, int payload_size);
    std::string parseHttp(const u_char* payload, int payload_size);
    std::string parseSmtp(const u_char* payload, int payload_size);
    std::string parseImap(const u_char* payload, int payload_size);
    std::string parsePop3(const u_char* payload, int payload_size);
    std::string parseSip(const u_char* payload, int payload_size);
    std::string parseRtp(const u_char* payload, int payload_size);
    std::string parseFtp(const u_char* payload, int payload_size);
    std::string parseIcmp(const icmp_header* icmp_hdr);
    std::string deviceName;
    pcap_t* handle;
    std::vector<pcap_if_t*> interfaces;
    static double last_timestamp;
    static std::map<std::pair<std::string, std::string>, FlowStats> flow_stats;
    static int packet_count;
};

#endif // PACKET_SNIFFER_H
