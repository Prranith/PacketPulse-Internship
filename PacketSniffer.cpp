////#define _CRT_SECURE_NO_WARNINGS
////#include "PacketSniffer.h"
////#include "Logger.h"
////#include "utils.h"
////#include <iostream>
////#include <iomanip>
////#include <sstream>
////#include <algorithm>
////
////// Define static members
////double PacketSniffer::last_timestamp = 0;
////std::map<std::pair<std::string, std::string>, FlowStats> PacketSniffer::flow_stats;
////int PacketSniffer::packet_count = 0;
////
////PacketSniffer::PacketSniffer(const std::string& deviceName) : deviceName(deviceName), handle(nullptr) {}
////
////PacketSniffer::~PacketSniffer() {
////    if (handle) {
////        pcap_close(handle);
////    }
////}
////
////void PacketSniffer::listInterfaces() {
////    pcap_if_t* alldevs;
////    char errbuf[PCAP_ERRBUF_SIZE];
////
////    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
////        std::cerr << "Error finding devices: " << errbuf << std::endl;
////        return;
////    }
////
////    interfaces.clear();
////    int i = 0;
////    for (pcap_if_t* d = alldevs; d; d = d->next) {
////        interfaces.push_back(d);
////        std::cout << i++ << ": " << (d->description ? d->description : "No description") << std::endl;
////    }
////    pcap_freealldevs(alldevs);
////}
////
////void PacketSniffer::startSniffing(int index) {
////    if (index < 0 || index >= interfaces.size()) {
////        std::cerr << "Invalid interface index." << std::endl;
////        return;
////    }
////
////    char errbuf[PCAP_ERRBUF_SIZE];
////    handle = pcap_open_live(interfaces[index]->name, 65535, 1, 1000, errbuf);
////    if (!handle) {
////        std::cerr << "Error opening device: " << errbuf << std::endl;
////        return;
////    }
////
////    if (pcap_set_promisc(handle, 1) != 0) {
////        std::cerr << "Error setting promiscuous mode: " << pcap_geterr(handle) << std::endl;
////    }
////
////    std::cout << "[*] Starting packet capture on " << interfaces[index]->name << "...\n" << std::endl;
////    int result = pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char*>(this));
////    if (result == -1) {
////        std::cerr << "pcap_loop error: " << pcap_geterr(handle) << std::endl;
////    }
////    else if (result == -2) {
////        std::cout << "pcap_loop terminated by user break." << std::endl;
////    }
////
////    pcap_close(handle);
////}
////
////std::string PacketSniffer::macToString(const u_char* mac) {
////    std::ostringstream oss;
////    oss << std::hex << std::setfill('0');
////    for (int i = 0; i < 6; ++i) {
////        oss << std::setw(2) << (int)mac[i];
////        if (i < 5) oss << ":";
////    }
////    return oss.str();
////}
////
////std::string PacketSniffer::ipToString(const in_addr& addr) {
////    char ip_str[INET_ADDRSTRLEN];
////    if (inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN)) {
////        return std::string(ip_str);
////    }
////    return "Invalid IPv4";
////}
////
////std::string PacketSniffer::ipv6ToString(const u_char* addr) {
////    char ip_str[INET6_ADDRSTRLEN];
////    if (inet_ntop(AF_INET6, addr, ip_str, INET6_ADDRSTRLEN)) {
////        return std::string(ip_str);
////    }
////    return "Invalid IPv6";
////}
////
////std::string PacketSniffer::getTcpFlags(const tcp_header* tcp_hdr) {
////    std::string flags;
////    if (tcp_hdr->flags & 0x01) flags += "FIN ";
////    if (tcp_hdr->flags & 0x02) flags += "SYN ";
////    if (tcp_hdr->flags & 0x04) flags += "RST ";
////    if (tcp_hdr->flags & 0x08) flags += "PSH ";
////    if (tcp_hdr->flags & 0x10) flags += "ACK ";
////    if (tcp_hdr->flags & 0x20) flags += "URG ";
////    return flags.empty() ? "None" : flags;
////}
////
////void PacketSniffer::logPayload(const u_char* payload, int payload_size, PacketMetadata& meta) {
////    std::ostringstream hex_ascii;
////    for (int i = 0; i < payload_size; i += 16) {
////        hex_ascii << std::setw(4) << std::setfill('0') << i << "  ";
////        for (int j = 0; j < 16; ++j) {
////            if (i + j < payload_size) {
////                unsigned char c = static_cast<unsigned char>(payload[i + j]);
////                hex_ascii << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
////            }
////            else {
////                hex_ascii << "   ";
////            }
////        }
////        hex_ascii << " ";
////        for (int j = 0; j < 16; ++j) {
////            if (i + j < payload_size) {
////                unsigned char c = static_cast<unsigned char>(payload[i + j]);
////                hex_ascii << (std::isprint(c) ? static_cast<char>(c) : '.');
////            }
////            else {
////                hex_ascii << " ";
////            }
////        }
////        hex_ascii << "\n";
////    }
////    meta.payload_hex_ascii = hex_ascii.str();
////}
////
////std::string PacketSniffer::parseIpOptions(const u_char* options, int options_len) {
////    std::ostringstream oss;
////    int pos = 0;
////    while (pos < options_len) {
////        uint8_t opt_code = options[pos];
////        if (opt_code == 0) { // End of options
////            oss << "End of Options";
////            break;
////        }
////        if (opt_code == 1) { // No Operation
////            oss << "NOP ";
////            pos++;
////            continue;
////        }
////        if (pos + 1 >= options_len) break;
////        uint8_t opt_len = options[pos + 1];
////        if (opt_len < 2 || pos + opt_len > options_len) break;
////        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len << " ";
////        pos += opt_len;
////    }
////    return oss.str();
////}
////
////std::string PacketSniffer::parseTcpOptions(const u_char* options, int options_len) {
////    std::ostringstream oss;
////    int pos = 0;
////    while (pos < options_len) {
////        uint8_t opt_code = options[pos];
////        if (opt_code == 0) { // End of options
////            oss << "End of Options";
////            break;
////        }
////        if (opt_code == 1) { // No Operation
////            oss << "NOP ";
////            pos++;
////            continue;
////        }
////        if (pos + 1 >= options_len) break;
////        uint8_t opt_len = options[pos + 1];
////        if (opt_len < 2 || pos + opt_len > options_len) break;
////        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len;
////        if (opt_code == 2) oss << " (MSS)";
////        else if (opt_code == 3) oss << " (Window Scale)";
////        else if (opt_code == 4) oss << " (SACK Permitted)";
////        else if (opt_code == 8) oss << " (Timestamps)";
////        oss << " ";
////        pos += opt_len;
////    }
////    return oss.str();
////}
////
////std::string PacketSniffer::parseApplicationPayload(const u_char* payload, int payload_size, const std::string& protocol, uint16_t src_port, uint16_t dst_port, PacketMetadata& meta) {
////    if (payload_size <= 0) return "No application data";
////
////    // Try protocol-specific parsing
////    if (protocol == "TCP" || protocol == "UDP") {
////        if (src_port == 80 || dst_port == 80 || src_port == 8080 || dst_port == 8080) {
////            meta.app_protocol = "HTTP";
////            meta.http_info = parseHttp(payload, payload_size);
////            if (!meta.http_info.empty()) return meta.http_info;
////        }
////        else if (src_port == 443 || dst_port == 443) {
////            meta.app_protocol = "TLS";
////            meta.tls_version = parseTlsVersion(payload, payload_size);
////            if (!meta.tls_version.empty()) return "TLS Version: " + meta.tls_version;
////        }
////        else if (src_port == 25 || dst_port == 25 || src_port == 587 || dst_port == 587) {
////            meta.app_protocol = "SMTP";
////            meta.smtp_info = parseSmtp(payload, payload_size);
////            if (!meta.smtp_info.empty()) return meta.smtp_info;
////        }
////        else if (src_port == 143 || dst_port == 143 || src_port == 993 || dst_port == 993) {
////            meta.app_protocol = "IMAP";
////            meta.imap_info = parseImap(payload, payload_size);
////            if (!meta.imap_info.empty()) return meta.imap_info;
////        }
////        else if (src_port == 110 || dst_port == 110 || src_port == 995 || dst_port == 995) {
////            meta.app_protocol = "POP3";
////            meta.pop3_info = parsePop3(payload, payload_size);
////            if (!meta.pop3_info.empty()) return meta.pop3_info;
////        }
////        else if (src_port == 5060 || dst_port == 5060 || src_port == 5061 || dst_port == 5061) {
////            meta.app_protocol = "SIP";
////            meta.sip_info = parseSip(payload, payload_size);
////            if (!meta.sip_info.empty()) return meta.sip_info;
////        }
////        else if ((src_port >= 10000 && src_port <= 65535 && dst_port >= 10000 && dst_port <= 65535) && protocol == "UDP") {
////            meta.app_protocol = "RTP";
////            meta.rtp_info = parseRtp(payload, payload_size);
////            if (!meta.rtp_info.empty()) return meta.rtp_info;
////        }
////        else if (src_port == 21 || dst_port == 21) {
////            meta.app_protocol = "FTP";
////            meta.ftp_info = parseFtp(payload, payload_size);
////            if (!meta.ftp_info.empty()) return meta.ftp_info;
////        }
////        else if (src_port == 53 || dst_port == 53 || src_port == 5353 || dst_port == 5353) {
////            meta.app_protocol = "DNS";
////            meta.dns_query = parseDnsQuery(payload, payload_size);
////            if (!meta.dns_query.empty()) return (src_port == 5353 || dst_port == 5353) ? "mDNS Query: " + meta.dns_query : "DNS Query: " + meta.dns_query;
////        }
////    }
////
////    // Fallback: Generic payload parsing for unknown protocols
////    meta.app_protocol = "Unknown";
////    std::string readable;
////    for (int i = 0; i < payload_size; ++i) {
////        unsigned char c = payload[i];
////        if (std::isprint(c)) {
////            readable += static_cast<char>(c);
////        }
////        else if (c == '\r' || c == '\n') {
////            readable += c;
////        }
////    }
////    meta.app_data = readable.empty() ? "No readable application data" : readable;
////    return meta.app_data;
////}
////
////std::string PacketSniffer::parseDnsQuery(const u_char* payload, int payload_size) {
////    if (payload_size < 12) return "";
////    std::string query;
////    int pos = 12;
////    while (pos < payload_size && payload[pos]) {
////        int seg_len = payload[pos++];
////        if (pos + seg_len >= payload_size) break;
////        for (int i = 0; i < seg_len; ++i) {
////            query += (char)payload[pos++];
////        }
////        query += ".";
////    }
////    if (!query.empty()) query.pop_back();
////    return query;
////}
////
////std::string PacketSniffer::parseTlsVersion(const u_char* payload, int payload_size) {
////    if (payload_size < 5 || payload[0] != 0x16) return "";
////    if (payload[1] == 0x03 && payload[2] == 0x03) return "TLS 1.2";
////    if (payload[1] == 0x03 && payload[2] == 0x04) return "TLS 1.3";
////    return "";
////}
////
////std::string PacketSniffer::parseHttp(const u_char* payload, int payload_size) {
////    std::string http_data(reinterpret_cast<const char*>(payload), payload_size);
////    std::size_t pos = http_data.find("\r\n");
////    if (pos != std::string::npos) {
////        std::string first_line = http_data.substr(0, pos);
////        if (first_line.find("HTTP/") != std::string::npos ||
////            first_line.find("GET ") != std::string::npos ||
////            first_line.find("POST ") != std::string::npos) {
////            return first_line;
////        }
////    }
////    return "";
////}
////
////std::string PacketSniffer::parseSmtp(const u_char* payload, int payload_size) {
////    std::string smtp_data(reinterpret_cast<const char*>(payload), payload_size);
////    std::ostringstream result;
////    std::size_t pos = 0;
////    if (smtp_data.find("HELO ") == 0 || smtp_data.find("EHLO ") == 0 ||
////        smtp_data.find("MAIL FROM:") == 0 || smtp_data.find("RCPT TO:") == 0) {
////        pos = smtp_data.find("\r\n");
////        if (pos != std::string::npos) {
////            result << smtp_data.substr(0, pos);
////        }
////        else {
////            result << smtp_data;
////        }
////    }
////    else if (smtp_data.find("DATA\r\n") == 0) {
////        pos = smtp_data.find("\r\n", 6);
////        if (pos != std::string::npos) {
////            std::string email_content = smtp_data.substr(pos + 2);
////            std::size_t header_end = email_content.find("\r\n\r\n");
////            if (header_end != std::string::npos) {
////                std::string headers = email_content.substr(0, header_end);
////                std::istringstream header_stream(headers);
////                std::string line;
////                while (std::getline(header_stream, line) && !line.empty()) {
////                    if (line.find("Subject: ") == 0 || line.find("From: ") == 0 || line.find("To: ") == 0) {
////                        result << "\n" << line;
////                    }
////                }
////                std::string body = email_content.substr(header_end + 4, 100);
////                if (!body.empty()) {
////                    result << "\nBody: " << body;
////                }
////            }
////        }
////    }
////    return result.str();
////}
////
////std::string PacketSniffer::parseImap(const u_char* payload, int payload_size) {
////    std::string imap_data(reinterpret_cast<const char*>(payload), payload_size);
////    std::size_t pos = imap_data.find("\r\n");
////    if (pos != std::string::npos) {
////        std::string command = imap_data.substr(0, pos);
////        if (command.find("LOGIN ") == 0 || command.find("FETCH ") == 0 ||
////            command.find("SELECT ") == 0 || command.find("LIST ") == 0) {
////            return command;
////        }
////        else if (imap_data.find("* OK ") == 0 || imap_data.find("* FLAGS ") == 0 ||
////            imap_data.find("* SEARCH ") == 0) {
////            return imap_data.substr(0, pos);
////        }
////    }
////    return "";
////}
////
////std::string PacketSniffer::parsePop3(const u_char* payload, int payload_size) {
////    std::string pop3_data(reinterpret_cast<const char*>(payload), payload_size);
////    std::size_t pos = pop3_data.find("\r\n");
////    if (pos != std::string::npos) {
////        std::string command = pop3_data.substr(0, pos);
////        if (command.find("USER ") == 0 || command.find("PASS ") == 0 ||
////            command.find("RETR ") == 0 || command.find("LIST ") == 0) {
////            return command;
////        }
////        else if (pop3_data.find("+OK ") == 0 || pop3_data.find("-ERR ") == 0) {
////            return pop3_data.substr(0, pos);
////        }
////    }
////    return "";
////}
////
////std::string PacketSniffer::parseSip(const u_char* payload, int payload_size) {
////    std::string sip_data(reinterpret_cast<const char*>(payload), payload_size);
////    std::size_t pos = sip_data.find("\r\n");
////    if (pos != std::string::npos) {
////        std::string first_line = sip_data.substr(0, pos);
////        if (first_line.find("INVITE ") == 0 || first_line.find("REGISTER ") == 0 ||
////            first_line.find("BYE ") == 0 || first_line.find("ACK ") == 0 ||
////            first_line.find("SIP/2.0 ") == 0) {
////            std::ostringstream result;
////            result << first_line;
////            std::size_t call_id_pos = sip_data.find("Call-ID: ");
////            if (call_id_pos != std::string::npos) {
////                std::size_t end_pos = sip_data.find("\r\n", call_id_pos);
////                if (end_pos != std::string::npos) {
////                    result << "\n" << sip_data.substr(call_id_pos, end_pos - call_id_pos);
////                }
////            }
////            return result.str();
////        }
////    }
////    return "";
////}
////
////std::string PacketSniffer::parseRtp(const u_char* payload, int payload_size) {
////    if (payload_size < 12) return "";
////    std::ostringstream result;
////    uint8_t version = (payload[0] >> 6) & 0x03;
////    uint8_t payload_type = payload[1] & 0x7F;
////    uint16_t sequence = ntohs(*reinterpret_cast<const uint16_t*>(&payload[2]));
////    uint32_t timestamp = ntohl(*reinterpret_cast<const uint32_t*>(&payload[4]));
////    uint32_t ssrc = ntohl(*reinterpret_cast<const uint32_t*>(&payload[8]));
////
////    if (version != 2) return "";
////
////    result << "Version: " << (int)version << ", Payload Type: " << (int)payload_type
////        << ", Sequence: " << sequence << ", Timestamp: " << timestamp
////        << ", SSRC: " << ssrc;
////
////    std::string codec;
////    switch (payload_type) {
////    case 0: codec = "PCMU"; break;
////    case 8: codec = "PCMA"; break;
////    case 18: codec = "G729"; break;
////    case 34: codec = "H263"; break;
////    default: codec = "Unknown"; break;
////    }
////    result << ", Codec: " << codec;
////    return result.str();
////}
////
////std::string PacketSniffer::parseFtp(const u_char* payload, int payload_size) {
////    std::string ftp_data(reinterpret_cast<const char*>(payload), payload_size);
////    if (ftp_data.find("USER ") == 0 || ftp_data.find("PASS ") == 0 ||
////        ftp_data.find("RETR ") == 0 || ftp_data.find("STOR ") == 0) {
////        std::size_t pos = ftp_data.find("\r\n");
////        if (pos != std::string::npos) {
////            return ftp_data.substr(0, pos);
////        }
////        return ftp_data;
////    }
////    return "";
////}
////
////std::string PacketSniffer::parseIcmp(const icmp_header* icmp_hdr) {
////    std::ostringstream oss;
////    oss << "Type: " << (int)icmp_hdr->type << ", Code: " << (int)icmp_hdr->code;
////    if (icmp_hdr->type == 8) oss << " (Echo Request)";
////    else if (icmp_hdr->type == 0) oss << " (Echo Reply)";
////    return oss.str();
////}
////
////void PacketSniffer::packetHandler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
////    PacketSniffer* sniffer = reinterpret_cast<PacketSniffer*>(param);
////    sniffer->printPacketDetails(header, pkt_data);
////}
////
////void PacketSniffer::printPacketDetails(const struct pcap_pkthdr* header, const u_char* pkt_data) {
////    PacketMetadata meta;
////    double timestamp = header->ts.tv_sec + header->ts.tv_usec / 1000000.0;
////    meta.timestamp = currentTimestamp(timestamp);
////    meta.inter_arrival = last_timestamp ? (timestamp - last_timestamp) : 0;
////    meta.captured_length = header->caplen;
////    meta.total_length = header->len;
////    last_timestamp = timestamp;
////
////    // Layer 1: Physical
////    meta.interface_name = interfaces.empty() ? "Unknown" : interfaces[0]->name;
////    meta.interface_description = interfaces.empty() ? "Unknown" : (interfaces[0]->description ? interfaces[0]->description : "No description");
////
////    std::ostringstream logStream;
////    logStream << "===== Packet #" << ++packet_count << " =====\n";
////    logStream << "[Layer 1 - Physical]\n";
////    logStream << "Interface Name: " << meta.interface_name << "\n";
////    logStream << "Interface Description: " << meta.interface_description << "\n";
////    logStream << "[Timing Information]\n";
////    logStream << "Timestamp: " << meta.timestamp << "\n";
////    logStream << "Inter-Arrival Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
////    logStream << "Captured Length: " << meta.captured_length << " bytes\n";
////    logStream << "Total Length: " << meta.total_length << " bytes\n";
////
////    std::cout << "\n===== Packet #" << packet_count << " =====\n";
////    std::cout << "[Layer 1 - Physical]\n";
////    std::cout << "Interface Name: " << meta.interface_name << "\n";
////    std::cout << "Interface Description: " << meta.interface_description << "\n";
////    std::cout << "[Timing Information]\n";
////    std::cout << "Timestamp: " << meta.timestamp << "\n";
////    std::cout << "Inter-Arrival Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
////    std::cout << "Captured Length: " << meta.captured_length << " bytes\n";
////    std::cout << "Total Length: " << meta.total_length << " bytes\n";
////
////    // Layer 2: Data Link
////    if (header->caplen < sizeof(ethernet_header)) {
////        meta.eth_type = "Unknown (Malformed)";
////        std::cout << "[Layer 2 - Data Link]\n";
////        std::cout << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
////        logStream << "[Layer 2 - Data Link]\n";
////        logStream << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
////        Logger::getInstance().logPacket(logStream.str());
////        return;
////    }
////
////    const ethernet_header* eth_hdr = reinterpret_cast<const ethernet_header*>(pkt_data);
////    meta.src_mac = macToString(eth_hdr->src_mac);
////    meta.dst_mac = macToString(eth_hdr->dest_mac);
////    meta.eth_type = "0x" + std::to_string(ntohs(eth_hdr->ether_type));
////
////    const u_char* next_layer = pkt_data + sizeof(ethernet_header);
////    meta.payload_size = header->caplen - sizeof(ethernet_header);
////
////    // Check for VLAN tag
////    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_VLAN) {
////        uint16_t vlan_id = ntohs(*(uint16_t*)(next_layer)) & 0x0FFF;
////        meta.vlan_tag = "VLAN ID: " + std::to_string(vlan_id);
////        meta.eth_type = "0x" + std::to_string(ntohs(*(uint16_t*)(next_layer + 2)));
////        next_layer += 4;
////        meta.payload_size -= 4;
////    }
////
////    std::cout << "[Layer 2 - Data Link]\n";
////    std::cout << "Source MAC: " << meta.src_mac << "\n";
////    std::cout << "Destination MAC: " << meta.dst_mac << "\n";
////    std::cout << "Ethernet Type: " << meta.eth_type << "\n";
////    std::cout << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
////    logStream << "[Layer 2 - Data Link]\n";
////    logStream << "Source MAC: " << meta.src_mac << "\n";
////    logStream << "Destination MAC: " << meta.dst_mac << "\n";
////    logStream << "Ethernet Type: " << meta.eth_type << "\n";
////    logStream << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
////
////    // Payload logging
////    if (meta.payload_size > 0) {
////        logPayload(next_layer, meta.payload_size, meta);
////        std::cout << "[Payload]\n";
////        std::cout << "Payload Size: " << meta.payload_size << " bytes\n";
////        std::cout << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
////        logStream << "[Payload]\n";
////        logStream << "Payload Size: " << meta.payload_size << " bytes\n";
////        logStream << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
////    }
////    else {
////        std::cout << "[Payload]\n";
////        std::cout << "No payload data\n";
////        logStream << "[Payload]\n";
////        logStream << "No payload data\n";
////    }
////
////    // Layer 3: Network
////    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IP && header->caplen >= sizeof(ethernet_header) + sizeof(ip_header)) {
////        const ip_header* ip_hdr = reinterpret_cast<const ip_header*>(next_layer);
////        int ip_hlen = (ip_hdr->ver_ihl & 0x0F) * 4;
////        meta.src_ip = ipToString(ip_hdr->src_addr);
////        meta.dst_ip = ipToString(ip_hdr->dst_addr);
////        meta.protocol = (ip_hdr->proto == IP_PROTO_TCP) ? "TCP" :
////            (ip_hdr->proto == IP_PROTO_UDP) ? "UDP" :
////            (ip_hdr->proto == IP_PROTO_ICMP) ? "ICMP" :
////            std::to_string(ip_hdr->proto);
////        meta.direction = (meta.src_ip.find("192.168.") == 0 || meta.src_ip.find("10.") == 0) ? "Outgoing" : "Incoming";
////
////        // Parse IP options
////        if (ip_hlen > sizeof(ip_header)) {
////            meta.ip_options = parseIpOptions(next_layer + sizeof(ip_header), ip_hlen - sizeof(ip_header));
////        }
////
////        std::cout << "[Layer 3 - Network]\n";
////        std::cout << "Source IP: " << meta.src_ip << "\n";
////        std::cout << "Destination IP: " << meta.dst_ip << "\n";
////        std::cout << "Protocol: " << meta.protocol << "\n";
////        std::cout << "Direction: " << meta.direction << "\n";
////        std::cout << "IP Header Length: " << ip_hlen << " bytes\n";
////        std::cout << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";
////        logStream << "[Layer 3 - Network]\n";
////        logStream << "Source IP: " << meta.src_ip << "\n";
////        logStream << "Destination IP: " << meta.dst_ip << "\n";
////        logStream << "Protocol: " << meta.protocol << "\n";
////        logStream << "Direction: " << meta.direction << "\n";
////        logStream << "IP Header Length: " << ip_hlen << " bytes\n";
////        logStream << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";
////
////        // Layer 4: Transport
////        const u_char* transport_data = next_layer + ip_hlen;
////        if (ip_hdr->proto == IP_PROTO_TCP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(tcp_header)) {
////            const tcp_header* tcp_hdr = reinterpret_cast<const tcp_header*>(transport_data);
////            meta.src_port = ntohs(tcp_hdr->src_port);
////            meta.dst_port = ntohs(tcp_hdr->dst_port);
////            meta.tcp_flags = getTcpFlags(tcp_hdr);
////            int tcp_hlen = ((tcp_hdr->offset_res >> 4) & 0x0F) * 4;
////            meta.checksum_status = "Not verified";
////
////            // Parse TCP options
////            if (tcp_hlen > sizeof(tcp_header)) {
////                meta.transport_options = parseTcpOptions(transport_data + sizeof(tcp_header), tcp_hlen - sizeof(tcp_header));
////            }
////
////            // Layer 5: Session
////            meta.session_id = std::to_string(ntohl(tcp_hdr->seq)) + ":" + std::to_string(ntohl(tcp_hdr->ack_seq));
////            meta.session_state = (tcp_hdr->flags & 0x02) ? "SYN Sent" :
////                (tcp_hdr->flags & 0x10 && !(tcp_hdr->flags & 0x01)) ? "Established" :
////                (tcp_hdr->flags & 0x01) ? "FIN Sent" : "Unknown";
////
////            std::cout << "[Layer 4 - Transport]\n";
////            std::cout << "Protocol: TCP\n";
////            std::cout << "Source Port: " << meta.src_port << "\n";
////            std::cout << "Destination Port: " << meta.dst_port << "\n";
////            std::cout << "TCP Flags: " << meta.tcp_flags << "\n";
////            std::cout << "Header Length: " << tcp_hlen << " bytes\n";
////            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
////            std::cout << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";
////            logStream << "[Layer 4 - Transport]\n";
////            logStream << "Protocol: TCP\n";
////            logStream << "Source Port: " << meta.src_port << "\n";
////            logStream << "Destination Port: " << meta.dst_port << "\n";
////            logStream << "TCP Flags: " << meta.tcp_flags << "\n";
////            logStream << "Header Length: " << tcp_hlen << " bytes\n";
////            logStream << "Checksum Status: " << meta.checksum_status << "\n";
////            logStream << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";
////
////            std::cout << "[Layer 5 - Session]\n";
////            std::cout << "Session ID: " << meta.session_id << "\n";
////            std::cout << "Session State: " << meta.session_state << "\n";
////            logStream << "[Layer 5 - Session]\n";
////            logStream << "Session ID: " << meta.session_id << "\n";
////            logStream << "Session State: " << meta.session_state << "\n";
////
////            // Layers 6-7: Presentation and Application
////            const u_char* app_data = transport_data + tcp_hlen;
////            int app_data_size = meta.payload_size - ip_hlen - tcp_hlen;
////            if (app_data_size > 0) {
////                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
////                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
////                meta.compression = "None";
////
////                std::cout << "[Layer 6 - Presentation]\n";
////                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
////                std::cout << "Compression: " << meta.compression << "\n";
////                logStream << "[Layer 6 - Presentation]\n";
////                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
////                logStream << "Compression: " << meta.compression << "\n";
////
////                std::cout << "[Layer 7 - Application]\n";
////                std::cout << "Protocol: " << meta.app_protocol << "\n";
////                std::cout << "Application Data: " << meta.app_data << "\n";
////                logStream << "[Layer 7 - Application]\n";
////                logStream << "Protocol: " << meta.app_protocol << "\n";
////                logStream << "Application Data: " << meta.app_data << "\n";
////            }
////        }
////        else if (ip_hdr->proto == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(udp_header)) {
////            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
////            meta.src_port = ntohs(udp_hdr->src_port);
////            meta.dst_port = ntohs(udp_hdr->dst_port);
////            meta.checksum_status = "Not verified";
////
////            // Layer 5: Session (UDP is connectionless, but protocols like SIP may have sessions)
////            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
////            meta.session_state = "Connectionless";
////
////            std::cout << "[Layer 4 - Transport]\n";
////            std::cout << "Protocol: UDP\n";
////            std::cout << "Source Port: " << meta.src_port << "\n";
////            std::cout << "Destination Port: " << meta.dst_port << "\n";
////            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
////            logStream << "[Layer 4 - Transport]\n";
////            logStream << "Protocol: UDP\n";
////            logStream << "Source Port: " << meta.src_port << "\n";
////            logStream << "Destination Port: " << meta.dst_port << "\n";
////            logStream << "Checksum Status: " << meta.checksum_status << "\n";
////
////            std::cout << "[Layer 5 - Session]\n";
////            std::cout << "Session ID: " << meta.session_id << "\n";
////            std::cout << "Session State: " << meta.session_state << "\n";
////            logStream << "[Layer 5 - Session]\n";
////            logStream << "Session ID: " << meta.session_id << "\n";
////            logStream << "Session State: " << meta.session_state << "\n";
////
////            // Layers 6-7: Presentation and Application
////            const u_char* app_data = transport_data + sizeof(udp_header);
////            int app_data_size = meta.payload_size - ip_hlen - sizeof(udp_header);
////            if (app_data_size > 0) {
////                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
////                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
////                meta.compression = "None";
////
////                std::cout << "[Layer 6 - Presentation]\n";
////                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
////                std::cout << "Compression: " << meta.compression << "\n";
////                logStream << "[Layer 6 - Presentation]\n";
////                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
////                logStream << "Compression: " << meta.compression << "\n";
////
////                std::cout << "[Layer 7 - Application]\n";
////                std::cout << "Protocol: " << meta.app_protocol << "\n";
////                std::cout << "Application Data: " << meta.app_data << "\n";
////                logStream << "[Layer 7 - Application]\n";
////                logStream << "Protocol: " << meta.app_protocol << "\n";
////                logStream << "Application Data: " << meta.app_data << "\n";
////            }
////        }
////        else if (ip_hdr->proto == IP_PROTO_ICMP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(icmp_header)) {
////            const icmp_header* icmp_hdr = reinterpret_cast<const icmp_header*>(transport_data);
////            meta.icmp_info = parseIcmp(icmp_hdr);
////            meta.checksum_status = "Not verified";
////            meta.session_id = "N/A (ICMP)";
////            meta.session_state = "Stateless";
////
////            std::cout << "[Layer 4 - Transport]\n";
////            std::cout << "Protocol: ICMP\n";
////            std::cout << "ICMP Info: " << meta.icmp_info << "\n";
////            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
////            logStream << "[Layer 4 - Transport]\n";
////            logStream << "Protocol: ICMP\n";
////            logStream << "ICMP Info: " << meta.icmp_info << "\n";
////            logStream << "Checksum Status: " << meta.checksum_status << "\n";
////
////            std::cout << "[Layer 5 - Session]\n";
////            std::cout << "Session ID: " << meta.session_id << "\n";
////            std::cout << "Session State: " << meta.session_state << "\n";
////            logStream << "[Layer 5 - Session]\n";
////            logStream << "Session ID: " << meta.session_id << "\n";
////            logStream << "Session State: " << meta.session_state << "\n";
////
////            std::cout << "[Layer 6 - Presentation]\n";
////            std::cout << "TLS Version: None\n";
////            std::cout << "Cipher Suite: None\n";
////            std::cout << "Compression: None\n";
////            logStream << "[Layer 6 - Presentation]\n";
////            logStream << "TLS Version: None\n";
////            logStream << "Cipher Suite: None\n";
////            logStream << "Compression: None\n";
////
////            std::cout << "[Layer 7 - Application]\n";
////            std::cout << "Protocol: ICMP\n";
////            std::cout << "Application Data: None\n";
////            logStream << "[Layer 7 - Application]\n";
////            logStream << "Protocol: ICMP\n";
////            logStream << "Application Data: None\n";
////        }
////
////        // Update flow stats
////        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
////        auto& stats = flow_stats[ip_pair];
////        stats.packet_count++;
////        stats.total_bytes += meta.payload_size;
////        if (stats.last_timestamp) {
////            double duration = timestamp - stats.last_timestamp;
////            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
////        }
////        stats.last_timestamp = timestamp;
////    }
////    else if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IPV6 && header->caplen >= sizeof(ethernet_header) + 40) {
////        const u_char* ipv6_data = next_layer;
////        u_char src_ipv6[16], dst_ipv6[16];
////        memcpy(src_ipv6, ipv6_data + 8, 16);
////        memcpy(dst_ipv6, ipv6_data + 24, 16);
////        meta.src_ip = ipv6ToString(src_ipv6);
////        meta.dst_ip = ipv6ToString(dst_ipv6);
////        meta.protocol = std::to_string(*(ipv6_data + 6));
////        meta.direction = (meta.src_ip.find("fe80::") == 0) ? "Outgoing" : "Incoming";
////
////        std::cout << "[Layer 3 - Network]\n";
////        std::cout << "Source IPv6: " << meta.src_ip << "\n";
////        std::cout << "Destination IPv6: " << meta.dst_ip << "\n";
////        std::cout << "Next Header: " << meta.protocol << "\n";
////        std::cout << "Direction: " << meta.direction << "\n";
////        logStream << "[Layer 3 - Network]\n";
////        logStream << "Source IPv6: " << meta.src_ip << "\n";
////        logStream << "Destination IPv6: " << meta.dst_ip << "\n";
////        logStream << "Next Header: " << meta.protocol << "\n";
////        logStream << "Direction: " << meta.direction << "\n";
////
////        uint8_t next_header = *(ipv6_data + 6);
////        int ipv6_hlen = 40;
////        const u_char* transport_data = ipv6_data + ipv6_hlen;
////        if (next_header == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ipv6_hlen + sizeof(udp_header)) {
////            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
////            meta.src_port = ntohs(udp_hdr->src_port);
////            meta.dst_port = ntohs(udp_hdr->dst_port);
////            meta.protocol = "UDP";
////            meta.checksum_status = "Not verified";
////            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
////            meta.session_state = "Connectionless";
////
////            std::cout << "[Layer 4 - Transport]\n";
////            std::cout << "Protocol: UDP\n";
////            std::cout << "Source Port: " << meta.src_port << "\n";
////            std::cout << "Destination Port: " << meta.dst_port << "\n";
////            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
////            logStream << "[Layer 4 - Transport]\n";
////            logStream << "Protocol: UDP\n";
////            logStream << "Source Port: " << meta.src_port << "\n";
////            logStream << "Destination Port: " << meta.dst_port << "\n";
////            logStream << "Checksum Status: " << meta.checksum_status << "\n";
////
////            std::cout << "[Layer 5 - Session]\n";
////            std::cout << "Session ID: " << meta.session_id << "\n";
////            std::cout << "Session State: " << meta.session_state << "\n";
////            logStream << "[Layer 5 - Session]\n";
////            logStream << "Session ID: " << meta.session_id << "\n";
////            logStream << "Session State: " << meta.session_state << "\n";
////
////            const u_char* app_data = transport_data + sizeof(udp_header);
////            int app_data_size = meta.payload_size - ipv6_hlen - sizeof(udp_header);
////            if (app_data_size > 0) {
////                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
////                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
////                meta.compression = "None";
////
////                std::cout << "[Layer 6 - Presentation]\n";
////                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
////                std::cout << "Compression: " << meta.compression << "\n";
////                logStream << "[Layer 6 - Presentation]\n";
////                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
////                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
////                logStream << "Compression: " << meta.compression << "\n";
////
////                std::cout << "[Layer 7 - Application]\n";
////                std::cout << "Protocol: " << meta.app_protocol << "\n";
////                std::cout << "Application Data: " << meta.app_data << "\n";
////                logStream << "[Layer 7 - Application]\n";
////                logStream << "Protocol: " << meta.app_protocol << "\n";
////                logStream << "Application Data: " << meta.app_data << "\n";
////            }
////        }
////
////        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
////        auto& stats = flow_stats[ip_pair];
////        stats.packet_count++;
////        stats.total_bytes += meta.payload_size;
////        if (stats.last_timestamp) {
////            double duration = timestamp - stats.last_timestamp;
////            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
////        }
////        stats.last_timestamp = timestamp;
////    }
////    else {
////        meta.protocol = "Non-IP";
////        meta.direction = "Unknown";
////        meta.session_id = "N/A";
////        meta.session_state = "N/A";
////        meta.app_protocol = "Unknown";
////        meta.app_data = "Non-IP packet";
////
////        std::cout << "[Layer 3 - Network]\n";
////        std::cout << "Non-IP packet, no IP addresses available\n";
////        std::cout << "Direction: " << meta.direction << "\n";
////        logStream << "[Layer 3 - Network]\n";
////        logStream << "Non-IP packet, no IP addresses available\n";
////        logStream << "Direction: " << meta.direction << "\n";
////
////        std::cout << "[Layer 4 - Transport]\n";
////        std::cout << "Protocol: None\n";
////        logStream << "[Layer 4 - Transport]\n";
////        logStream << "Protocol: None\n";
////
////        std::cout << "[Layer 5 - Session]\n";
////        std::cout << "Session ID: " << meta.session_id << "\n";
////        std::cout << "Session State: " << meta.session_state << "\n";
////        logStream << "[Layer 5 - Session]\n";
////        logStream << "Session ID: " << meta.session_id << "\n";
////        logStream << "Session State: " << meta.session_state << "\n";
////
////        std::cout << "[Layer 6 - Presentation]\n";
////        std::cout << "TLS Version: None\n";
////        std::cout << "Cipher Suite: None\n";
////        std::cout << "Compression: None\n";
////        logStream << "[Layer 6 - Presentation]\n";
////        logStream << "TLS Version: None\n";
////        logStream << "Cipher Suite: None\n";
////        logStream << "Compression: None\n";
////
////        std::cout << "[Layer 7 - Application]\n";
////        std::cout << "Protocol: " << meta.app_protocol << "\n";
////        std::cout << "Application Data: " << meta.app_data << "\n";
////        logStream << "[Layer 7 - Application]\n";
////        logStream << "Protocol: " << meta.app_protocol << "\n";
////        logStream << "Application Data: " << meta.app_data << "\n";
////    }
////
////    Logger::getInstance().logPacket(logStream.str());
////
////    // Log flow stats every 100 packets
////    if (packet_count % 100 == 0) {
////        std::ostringstream flowStream;
////        for (const auto& [pair, stats] : flow_stats) {
////            flowStream << "Flow: " << pair.first << " -> " << pair.second << "\n";
////            flowStream << "Packets: " << stats.packet_count << ", Bytes: "
////                << stats.total_bytes << ", Rate: " << stats.byte_rate << " B/s\n";
////        }
////        Logger::getInstance().logFlowStats(flowStream.str());
////    }
////
////    std::cout << std::dec;
////}
//#define _CRT_SECURE_NO_WARNINGS
//#include "PacketSniffer.h"
//#include "Logger.h"
//#include "utils.h"
//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include <algorithm>
//
//// Define static members
//double PacketSniffer::last_timestamp = 0;
//std::map<std::pair<std::string, std::string>, FlowStats> PacketSniffer::flow_stats;
//int PacketSniffer::packet_count = 0;
//
//PacketSniffer::PacketSniffer(const std::string& deviceName) : deviceName(deviceName), handle(nullptr) {}
//
//PacketSniffer::~PacketSniffer() {
//    if (handle) {
//        pcap_close(handle);
//    }
//}
//
//void PacketSniffer::listInterfaces() {
//    pcap_if_t* alldevs;
//    char errbuf[PCAP_ERRBUF_SIZE];
//
//    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
//        std::cerr << "Error finding devices: " << errbuf << std::endl;
//        return;
//    }
//
//    interfaces.clear();
//    int i = 0;
//    for (pcap_if_t* d = alldevs; d; d = d->next) {
//        interfaces.push_back(d);
//        std::cout << i++ << ": " << (d->description ? d->description : "No description") << std::endl;
//    }
//    pcap_freealldevs(alldevs);
//}
//
//void PacketSniffer::startSniffing(int index) {
//    if (index < 0 || index >= interfaces.size()) {
//        std::cerr << "Invalid interface index." << std::endl;
//        return;
//    }
//
//    char errbuf[PCAP_ERRBUF_SIZE];
//    handle = pcap_open_live(interfaces[index]->name, 65535, 1, 1000, errbuf);
//    if (!handle) {
//        std::cerr << "Error opening device: " << errbuf << std::endl;
//        return;
//    }
//
//    if (pcap_set_promisc(handle, 1) != 0) {
//        std::cerr << "Error setting promiscuous mode: " << pcap_geterr(handle) << std::endl;
//    }
//
//    std::cout << "[*] Starting packet capture on " << interfaces[index]->name << "...\n" << std::endl;
//    int result = pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char*>(this));
//    if (result == -1) {
//        std::cerr << "pcap_loop error: " << pcap_geterr(handle) << std::endl;
//    }
//    else if (result == -2) {
//        std::cout << "pcap_loop terminated by user break." << std::endl;
//    }
//
//    pcap_close(handle);
//}
//
//std::string PacketSniffer::macToString(const u_char* mac) {
//    std::ostringstream oss;
//    oss << std::hex << std::setfill('0');
//    for (int i = 0; i < 6; ++i) {
//        oss << std::setw(2) << (int)mac[i];
//        if (i < 5) oss << ":";
//    }
//    return oss.str();
//}
//
//std::string PacketSniffer::ipToString(const in_addr& addr) {
//    char ip_str[INET_ADDRSTRLEN];
//    if (inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN)) {
//        return std::string(ip_str);
//    }
//    return "Invalid IPv4";
//}
//
//std::string PacketSniffer::ipv6ToString(const u_char* addr) {
//    char ip_str[INET6_ADDRSTRLEN];
//    if (inet_ntop(AF_INET6, addr, ip_str, INET6_ADDRSTRLEN)) {
//        return std::string(ip_str);
//    }
//    return "Invalid IPv6";
//}
//
//std::string PacketSniffer::getTcpFlags(const tcp_header* tcp_hdr) {
//    std::string flags;
//    if (tcp_hdr->flags & 0x01) flags += "FIN ";
//    if (tcp_hdr->flags & 0x02) flags += "SYN ";
//    if (tcp_hdr->flags & 0x04) flags += "RST ";
//    if (tcp_hdr->flags & 0x08) flags += "PSH ";
//    if (tcp_hdr->flags & 0x10) flags += "ACK ";
//    if (tcp_hdr->flags & 0x20) flags += "URG ";
//    return flags.empty() ? "None" : flags;
//}
//
//void PacketSniffer::logPayload(const u_char* payload, int payload_size, PacketMetadata& meta) {
//    std::ostringstream hex_ascii;
//    for (int i = 0; i < payload_size; i += 16) {
//        hex_ascii << std::setw(4) << std::setfill('0') << i << "  ";
//        for (int j = 0; j < 16; ++j) {
//            if (i + j < payload_size) {
//                unsigned char c = static_cast<unsigned char>(payload[i + j]);
//                hex_ascii << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
//            }
//            else {
//                hex_ascii << "   ";
//            }
//        }
//        hex_ascii << " ";
//        for (int j = 0; j < 16; ++j) {
//            if (i + j < payload_size) {
//                unsigned char c = static_cast<unsigned char>(payload[i + j]);
//                hex_ascii << (std::isprint(c) ? static_cast<char>(c) : '.');
//            }
//            else {
//                hex_ascii << " ";
//            }
//        }
//        hex_ascii << "\n";
//    }
//    meta.payload_hex_ascii = hex_ascii.str();
//}
//
//std::string PacketSniffer::parseIpOptions(const u_char* options, int options_len) {
//    std::ostringstream oss;
//    int pos = 0;
//    while (pos < options_len) {
//        uint8_t opt_code = options[pos];
//        if (opt_code == 0) { // End of options
//            oss << "End of Options";
//            break;
//        }
//        if (opt_code == 1) { // No Operation
//            oss << "NOP ";
//            pos++;
//            continue;
//        }
//        if (pos + 1 >= options_len) break;
//        uint8_t opt_len = options[pos + 1];
//        if (opt_len < 2 || pos + opt_len > options_len) break;
//        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len << " ";
//        pos += opt_len;
//    }
//    return oss.str();
//}
//
//std::string PacketSniffer::parseTcpOptions(const u_char* options, int options_len) {
//    std::ostringstream oss;
//    int pos = 0;
//    while (pos < options_len) {
//        uint8_t opt_code = options[pos];
//        if (opt_code == 0) { // End of options
//            oss << "End of Options";
//            break;
//        }
//        if (opt_code == 1) { // No Operation
//            oss << "NOP ";
//            pos++;
//            continue;
//        }
//        if (pos + 1 >= options_len) break;
//        uint8_t opt_len = options[pos + 1];
//        if (opt_len < 2 || pos + opt_len > options_len) break;
//        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len;
//        if (opt_code == 2) oss << " (MSS)";
//        else if (opt_code == 3) oss << " (Window Scale)";
//        else if (opt_code == 4) oss << " (SACK Permitted)";
//        else if (opt_code == 8) oss << " (Timestamps)";
//        oss << " ";
//        pos += opt_len;
//    }
//    return oss.str();
//}
//
//std::string PacketSniffer::parseApplicationPayload(const u_char* payload, int payload_size, const std::string& protocol, uint16_t src_port, uint16_t dst_port, PacketMetadata& meta) {
//    if (payload_size <= 0) return "No application data";
//
//    // Try protocol-specific parsing
//    if (protocol == "TCP" || protocol == "UDP") {
//        if (src_port == 80 || dst_port == 80 || src_port == 8080 || dst_port == 8080) {
//            meta.app_protocol = "HTTP";
//            meta.http_info = parseHttp(payload, payload_size);
//            if (!meta.http_info.empty()) return meta.http_info;
//        }
//        else if (src_port == 443 || dst_port == 443) {
//            meta.app_protocol = "TLS";
//            meta.tls_version = parseTlsVersion(payload, payload_size);
//            if (!meta.tls_version.empty()) return "TLS Version: " + meta.tls_version;
//        }
//        else if (src_port == 25 || dst_port == 25 || src_port == 587 || dst_port == 587) {
//            meta.app_protocol = "SMTP";
//            meta.smtp_info = parseSmtp(payload, payload_size);
//            if (!meta.smtp_info.empty()) return meta.smtp_info;
//        }
//        else if (src_port == 143 || dst_port == 143 || src_port == 993 || dst_port == 993) {
//            meta.app_protocol = "IMAP";
//            meta.imap_info = parseImap(payload, payload_size);
//            if (!meta.imap_info.empty()) return meta.imap_info;
//        }
//        else if (src_port == 110 || dst_port == 110 || src_port == 995 || dst_port == 995) {
//            meta.app_protocol = "POP3";
//            meta.pop3_info = parsePop3(payload, payload_size);
//            if (!meta.pop3_info.empty()) return meta.pop3_info;
//        }
//        else if (src_port == 5060 || dst_port == 5060 || src_port == 5061 || dst_port == 5061) {
//            meta.app_protocol = "SIP";
//            meta.sip_info = parseSip(payload, payload_size);
//            if (!meta.sip_info.empty()) return meta.sip_info;
//        }
//        else if ((src_port >= 10000 && src_port <= 65535 && dst_port >= 10000 && dst_port <= 65535) && protocol == "UDP") {
//            meta.app_protocol = "RTP";
//            meta.rtp_info = parseRtp(payload, payload_size);
//            if (!meta.rtp_info.empty()) return meta.rtp_info;
//        }
//        else if (src_port == 21 || dst_port == 21) {
//            meta.app_protocol = "FTP";
//            meta.ftp_info = parseFtp(payload, payload_size);
//            if (!meta.ftp_info.empty()) return meta.ftp_info;
//        }
//        else if (src_port == 53 || dst_port == 53 || src_port == 5353 || dst_port == 5353) {
//            meta.app_protocol = "DNS";
//            meta.dns_query = parseDnsQuery(payload, payload_size);
//            if (!meta.dns_query.empty()) return (src_port == 5353 || dst_port == 5353) ? "mDNS Query: " + meta.dns_query : "DNS Query: " + meta.dns_query;
//        }
//    }
//
//    // Fallback: Generic payload parsing for unknown protocols
//    meta.app_protocol = "Unknown";
//    std::string readable;
//    for (int i = 0; i < payload_size; ++i) {
//        unsigned char c = payload[i];
//        if (std::isprint(c)) {
//            readable += static_cast<char>(c);
//        }
//        else if (c == '\r' || c == '\n') {
//            readable += c;
//        }
//    }
//    meta.app_data = readable.empty() ? "No readable application data" : readable;
//    return meta.app_data;
//}
//
//std::string PacketSniffer::parseDnsQuery(const u_char* payload, int payload_size) {
//    if (payload_size < 12) return "";
//    std::string query;
//    int pos = 12;
//    while (pos < payload_size && payload[pos]) {
//        int seg_len = payload[pos++];
//        if (pos + seg_len >= payload_size) break;
//        for (int i = 0; i < seg_len; ++i) {
//            query += (char)payload[pos++];
//        }
//        query += ".";
//    }
//    if (!query.empty()) query.pop_back();
//    return query;
//}
//
//std::string PacketSniffer::parseTlsVersion(const u_char* payload, int payload_size) {
//    if (payload_size < 5 || payload[0] != 0x16) return "";
//    if (payload[1] == 0x03 && payload[2] == 0x03) return "TLS 1.2";
//    if (payload[1] == 0x03 && payload[2] == 0x04) return "TLS 1.3";
//    return "";
//}
//
//std::string PacketSniffer::parseHttp(const u_char* payload, int payload_size) {
//    std::string http_data(reinterpret_cast<const char*>(payload), payload_size);
//    std::size_t pos = http_data.find("\r\n");
//    if (pos != std::string::npos) {
//        std::string first_line = http_data.substr(0, pos);
//        if (first_line.find("HTTP/") != std::string::npos ||
//            first_line.find("GET ") != std::string::npos ||
//            first_line.find("POST ") != std::string::npos) {
//            return first_line;
//        }
//    }
//    return "";
//}
//
//std::string PacketSniffer::parseSmtp(const u_char* payload, int payload_size) {
//    std::string smtp_data(reinterpret_cast<const char*>(payload), payload_size);
//    std::ostringstream result;
//    std::size_t pos = 0;
//    if (smtp_data.find("HELO ") == 0 || smtp_data.find("EHLO ") == 0 ||
//        smtp_data.find("MAIL FROM:") == 0 || smtp_data.find("RCPT TO:") == 0) {
//        pos = smtp_data.find("\r\n");
//        if (pos != std::string::npos) {
//            result << smtp_data.substr(0, pos);
//        }
//        else {
//            result << smtp_data;
//        }
//    }
//    else if (smtp_data.find("DATA\r\n") == 0) {
//        pos = smtp_data.find("\r\n", 6);
//        if (pos != std::string::npos) {
//            std::string email_content = smtp_data.substr(pos + 2);
//            std::size_t header_end = email_content.find("\r\n\r\n");
//            if (header_end != std::string::npos) {
//                std::string headers = email_content.substr(0, header_end);
//                std::istringstream header_stream(headers);
//                std::string line;
//                while (std::getline(header_stream, line) && !line.empty()) {
//                    if (line.find("Subject: ") == 0 || line.find("From: ") == 0 || line.find("To: ") == 0) {
//                        result << "\n" << line;
//                    }
//                }
//                std::string body = email_content.substr(header_end + 4, 100);
//                if (!body.empty()) {
//                    result << "\nBody: " << body;
//                }
//            }
//        }
//    }
//    return result.str();
//}
//
//std::string PacketSniffer::parseImap(const u_char* payload, int payload_size) {
//    std::string imap_data(reinterpret_cast<const char*>(payload), payload_size);
//    std::size_t pos = imap_data.find("\r\n");
//    if (pos != std::string::npos) {
//        std::string command = imap_data.substr(0, pos);
//        if (command.find("LOGIN ") == 0 || command.find("FETCH ") == 0 ||
//            command.find("SELECT ") == 0 || command.find("LIST ") == 0) {
//            return command;
//        }
//        else if (imap_data.find("* OK ") == 0 || imap_data.find("* FLAGS ") == 0 ||
//            imap_data.find("* SEARCH ") == 0) {
//            return imap_data.substr(0, pos);
//        }
//    }
//    return "";
//}
//
//std::string PacketSniffer::parsePop3(const u_char* payload, int payload_size) {
//    std::string pop3_data(reinterpret_cast<const char*>(payload), payload_size);
//    std::size_t pos = pop3_data.find("\r\n");
//    if (pos != std::string::npos) {
//        std::string command = pop3_data.substr(0, pos);
//        if (command.find("USER ") == 0 || command.find("PASS ") == 0 ||
//            command.find("RETR ") == 0 || command.find("LIST ") == 0) {
//            return command;
//        }
//        else if (pop3_data.find("+OK ") == 0 || pop3_data.find("-ERR ") == 0) {
//            return pop3_data.substr(0, pos);
//        }
//    }
//    return "";
//}
//
//std::string PacketSniffer::parseSip(const u_char* payload, int payload_size) {
//    std::string sip_data(reinterpret_cast<const char*>(payload), payload_size);
//    std::size_t pos = sip_data.find("\r\n");
//    if (pos != std::string::npos) {
//        std::string first_line = sip_data.substr(0, pos);
//        if (first_line.find("INVITE ") == 0 || first_line.find("REGISTER ") == 0 ||
//            first_line.find("BYE ") == 0 || first_line.find("ACK ") == 0 ||
//            first_line.find("SIP/2.0 ") == 0) {
//            std::ostringstream result;
//            result << first_line;
//            std::size_t call_id_pos = sip_data.find("Call-ID: ");
//            if (call_id_pos != std::string::npos) {
//                std::size_t end_pos = sip_data.find("\r\n", call_id_pos);
//                if (end_pos != std::string::npos) {
//                    result << "\n" << sip_data.substr(call_id_pos, end_pos - call_id_pos);
//                }
//            }
//            return result.str();
//        }
//    }
//    return "";
//}
//
//std::string PacketSniffer::parseRtp(const u_char* payload, int payload_size) {
//    if (payload_size < 12) return "";
//    std::ostringstream result;
//    uint8_t version = (payload[0] >> 6) & 0x03;
//    uint8_t payload_type = payload[1] & 0x7F;
//    uint16_t sequence = ntohs(*reinterpret_cast<const uint16_t*>(&payload[2]));
//    uint32_t timestamp = ntohl(*reinterpret_cast<const uint32_t*>(&payload[4]));
//    uint32_t ssrc = ntohl(*reinterpret_cast<const uint32_t*>(&payload[8]));
//
//    if (version != 2) return "";
//
//    result << "Version: " << (int)version << ", Payload Type: " << (int)payload_type
//        << ", Sequence: " << sequence << ", Timestamp: " << timestamp
//        << ", SSRC: " << ssrc;
//
//    std::string codec;
//    switch (payload_type) {
//    case 0: codec = "PCMU"; break;
//    case 8: codec = "PCMA"; break;
//    case 18: codec = "G729"; break;
//    case 34: codec = "H263"; break;
//    default: codec = "Unknown"; break;
//    }
//    result << ", Codec: " << codec;
//    return result.str();
//}
//
//std::string PacketSniffer::parseFtp(const u_char* payload, int payload_size) {
//    std::string ftp_data(reinterpret_cast<const char*>(payload), payload_size);
//    if (ftp_data.find("USER ") == 0 || ftp_data.find("PASS ") == 0 ||
//        ftp_data.find("RETR ") == 0 || ftp_data.find("STOR ") == 0) {
//        std::size_t pos = ftp_data.find("\r\n");
//        if (pos != std::string::npos) {
//            return ftp_data.substr(0, pos);
//        }
//        return ftp_data;
//    }
//    return "";
//}
//
//std::string PacketSniffer::parseIcmp(const icmp_header* icmp_hdr) {
//    std::ostringstream oss;
//    oss << "Type: " << (int)icmp_hdr->type << ", Code: " << (int)icmp_hdr->code;
//    if (icmp_hdr->type == 8) oss << " (Echo Request)";
//    else if (icmp_hdr->type == 0) oss << " (Echo Reply)";
//    return oss.str();
//}
//
//void PacketSniffer::packetHandler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
//    PacketSniffer* sniffer = reinterpret_cast<PacketSniffer*>(param);
//    sniffer->printPacketDetails(header, pkt_data);
//}
//
//void PacketSniffer::printPacketDetails(const struct pcap_pkthdr* header, const u_char* pkt_data) {
//    PacketMetadata meta;
//    double timestamp = header->ts.tv_sec + header->ts.tv_usec / 1000000.0;
//    meta.timestamp = currentTimestamp(timestamp);
//    meta.inter_arrival = last_timestamp ? (timestamp - last_timestamp) : 0;
//    meta.captured_length = header->caplen;
//    meta.total_length = header->len;
//    last_timestamp = timestamp;
//
//    // Layer 1: Physical
//    meta.interface_name = interfaces.empty() ? "Unknown" : interfaces[0]->name;
//    meta.interface_description = interfaces.empty() ? "Unknown" : (interfaces[0]->description ? interfaces[0]->description : "No description");
//
//    std::ostringstream logStream;
//    logStream << "===== Packet #" << ++packet_count << " =====\n";
//    logStream << "[Layer 1 - Physical]\n";
//    logStream << "Interface Name: " << meta.interface_name << "\n";
//    logStream << "Interface Description: " << meta.interface_description << "\n";
//    logStream << "[Timing Information]\n";
//    logStream << "Timestamp: " << meta.timestamp << "\n";
//    logStream << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
//    logStream << "Captured Length: " << meta.captured_length << " bytes\n";
//    logStream << "Total Length: " << meta.total_length << " bytes\n";
//
//    std::cout << "\n===== Packet #" << packet_count << " =====\n";
//    std::cout << "[Layer 1 - Physical]\n";
//    std::cout << "Interface Name: " << meta.interface_name << "\n";
//    std::cout << "Interface Description: " << meta.interface_description << "\n";
//    std::cout << "[Timing Information]\n";
//    std::cout << "Timestamp: " << meta.timestamp << "\n";
//    std::cout << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
//    std::cout << "Captured Length: " << meta.captured_length << " bytes\n";
//    std::cout << "Total Length: " << meta.total_length << " bytes\n";
//
//    // Layer 2: Data Link
//    if (header->caplen < sizeof(ethernet_header)) {
//        meta.eth_type = "Unknown (Malformed)";
//        std::cout << "[Layer 2 - Data Link]\n";
//        std::cout << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
//        logStream << "[Layer 2 - Data Link]\n";
//        logStream << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
//        Logger::getInstance().logPacket(logStream.str());
//        return;
//    }
//
//    const ethernet_header* eth_hdr = reinterpret_cast<const ethernet_header*>(pkt_data);
//    meta.src_mac = macToString(eth_hdr->src_mac);
//    meta.dst_mac = macToString(eth_hdr->dest_mac);
//    meta.eth_type = "0x" + std::to_string(ntohs(eth_hdr->ether_type));
//
//    const u_char* next_layer = pkt_data + sizeof(ethernet_header);
//    meta.payload_size = header->caplen - sizeof(ethernet_header);
//
//    // Check for VLAN tag
//    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_VLAN) {
//        uint16_t vlan_id = ntohs(*(uint16_t*)(next_layer)) & 0x0FFF;
//        meta.vlan_tag = "VLAN ID: " + std::to_string(vlan_id);
//        meta.eth_type = "0x" + std::to_string(ntohs(*(uint16_t*)(next_layer + 2)));
//        next_layer += 4;
//        meta.payload_size -= 4;
//    }
//
//    std::cout << "[Layer 2 - Data Link]\n";
//    std::cout << "Source MAC: " << meta.src_mac << "\n";
//    std::cout << "Destination MAC: " << meta.dst_mac << "\n";
//    std::cout << "Ethernet Type: " << meta.eth_type << "\n";
//    std::cout << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
//    logStream << "[Layer 2 - Data Link]\n";
//    logStream << "Source MAC: " << meta.src_mac << "\n";
//    logStream << "Destination MAC: " << meta.dst_mac << "\n";
//    logStream << "Ethernet Type: " << meta.eth_type << "\n";
//    logStream << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
//
//    // Payload logging
//    if (meta.payload_size > 0) {
//        logPayload(next_layer, meta.payload_size, meta);
//        std::cout << "[Payload]\n";
//        std::cout << "Payload Size: " << meta.payload_size << " bytes\n";
//        std::cout << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
//        logStream << "[Payload]\n";
//        logStream << "Payload Size: " << meta.payload_size << " bytes\n";
//        logStream << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
//    }
//    else {
//        std::cout << "[Payload]\n";
//        std::cout << "No payload data\n";
//        logStream << "[Payload]\n";
//        logStream << "No payload data\n";
//    }
//
//    // Layer 3: Network
//    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IP && header->caplen >= sizeof(ethernet_header) + sizeof(ip_header)) {
//        const ip_header* ip_hdr = reinterpret_cast<const ip_header*>(next_layer);
//        int ip_hlen = (ip_hdr->ver_ihl & 0x0F) * 4;
//        meta.src_ip = ipToString(ip_hdr->src_addr);
//        meta.dst_ip = ipToString(ip_hdr->dst_addr);
//        meta.protocol = (ip_hdr->proto == IP_PROTO_TCP) ? "TCP" :
//            (ip_hdr->proto == IP_PROTO_UDP) ? "UDP" :
//            (ip_hdr->proto == IP_PROTO_ICMP) ? "ICMP" :
//            std::to_string(ip_hdr->proto);
//        meta.direction = (meta.src_ip.find("192.168.") == 0 || meta.src_ip.find("10.") == 0) ? "Outgoing" : "Incoming";
//
//        // Parse IP options
//        if (ip_hlen > sizeof(ip_header)) {
//            meta.ip_options = parseIpOptions(next_layer + sizeof(ip_header), ip_hlen - sizeof(ip_header));
//        }
//
//        std::cout << "[Layer 3 - Network]\n";
//        std::cout << "Source IP: " << meta.src_ip << "\n";
//        std::cout << "Destination IP: " << meta.dst_ip << "\n";
//        std::cout << "Protocol: " << meta.protocol << "\n";
//        std::cout << "Direction: " << meta.direction << "\n";
//        std::cout << "IP Header Length: " << ip_hlen << " bytes\n";
//        std::cout << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";
//        logStream << "[Layer 3 - Network]\n";
//        logStream << "Source IP: " << meta.src_ip << "\n";
//        logStream << "Destination IP: " << meta.dst_ip << "\n";
//        logStream << "Protocol: " << meta.protocol << "\n";
//        logStream << "Direction: " << meta.direction << "\n";
//        logStream << "IP Header Length: " << ip_hlen << " bytes\n";
//        logStream << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";
//
//        // Layer 4: Transport
//        const u_char* transport_data = next_layer + ip_hlen;
//        if (ip_hdr->proto == IP_PROTO_TCP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(tcp_header)) {
//            const tcp_header* tcp_hdr = reinterpret_cast<const tcp_header*>(transport_data);
//            meta.src_port = ntohs(tcp_hdr->src_port);
//            meta.dst_port = ntohs(tcp_hdr->dst_port);
//            meta.tcp_flags = getTcpFlags(tcp_hdr);
//            int tcp_hlen = ((tcp_hdr->offset_res >> 4) & 0x0F) * 4;
//            meta.checksum_status = "Not verified";
//
//            // Parse TCP options
//            if (tcp_hlen > sizeof(tcp_header)) {
//                meta.transport_options = parseTcpOptions(transport_data + sizeof(tcp_header), tcp_hlen - sizeof(tcp_header));
//            }
//
//            // Layer 5: Session
//            meta.session_id = std::to_string(ntohl(tcp_hdr->seq)) + ":" + std::to_string(ntohl(tcp_hdr->ack_seq));
//            meta.session_state = (tcp_hdr->flags & 0x02) ? "SYN Sent" :
//                (tcp_hdr->flags & 0x10 && !(tcp_hdr->flags & 0x01)) ? "Established" :
//                (tcp_hdr->flags & 0x01) ? "FIN Sent" : "Unknown";
//
//            std::cout << "[Layer 4 - Transport]\n";
//            std::cout << "Protocol: TCP\n";
//            std::cout << "Source Port: " << meta.src_port << "\n";
//            std::cout << "Destination Port: " << meta.dst_port << "\n";
//            std::cout << "TCP Flags: " << meta.tcp_flags << "\n";
//            std::cout << "Header Length: " << tcp_hlen << " bytes\n";
//            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
//            std::cout << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";
//            logStream << "[Layer 4 - Transport]\n";
//            logStream << "Protocol: TCP\n";
//            logStream << "Source Port: " << meta.src_port << "\n";
//            logStream << "Destination Port: " << meta.dst_port << "\n";
//            logStream << "TCP Flags: " << meta.tcp_flags << "\n";
//            logStream << "Header Length: " << tcp_hlen << " bytes\n";
//            logStream << "Checksum Status: " << meta.checksum_status << "\n";
//            logStream << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";
//
//            std::cout << "[Layer 5 - Session]\n";
//            std::cout << "Session ID: " << meta.session_id << "\n";
//            std::cout << "Session State: " << meta.session_state << "\n";
//            logStream << "[Layer 5 - Session]\n";
//            logStream << "Session ID: " << meta.session_id << "\n";
//            logStream << "Session State: " << meta.session_state << "\n";
//
//            // Layers 6-7: Presentation and Application
//            const u_char* app_data = transport_data + tcp_hlen;
//            int app_data_size = meta.payload_size - ip_hlen - tcp_hlen;
//            if (app_data_size > 0) {
//                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
//                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
//                meta.compression = "None";
//
//                std::cout << "[Layer 6 - Presentation]\n";
//                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
//                std::cout << "Compression: " << meta.compression << "\n";
//                logStream << "[Layer 6 - Presentation]\n";
//                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
//                logStream << "Compression: " << meta.compression << "\n";
//
//                std::cout << "[Layer 7 - Application]\n";
//                std::cout << "Protocol: " << meta.app_protocol << "\n";
//                std::cout << "Application Data: " << meta.app_data << "\n";
//                logStream << "[Layer 7 - Application]\n";
//                logStream << "Protocol: " << meta.app_protocol << "\n";
//                logStream << "Application Data: " << meta.app_data << "\n";
//            }
//        }
//        else if (ip_hdr->proto == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(udp_header)) {
//            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
//            meta.src_port = ntohs(udp_hdr->src_port);
//            meta.dst_port = ntohs(udp_hdr->dst_port);
//            meta.checksum_status = "Not verified";
//
//            // Layer 5: Session (UDP is connectionless, but protocols like SIP may have sessions)
//            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
//            meta.session_state = "Connectionless";
//
//            std::cout << "[Layer 4 - Transport]\n";
//            std::cout << "Protocol: UDP\n";
//            std::cout << "Source Port: " << meta.src_port << "\n";
//            std::cout << "Destination Port: " << meta.dst_port << "\n";
//            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
//            logStream << "[Layer 4 - Transport]\n";
//            logStream << "Protocol: UDP\n";
//            logStream << "Source Port: " << meta.src_port << "\n";
//            logStream << "Destination Port: " << meta.dst_port << "\n";
//            logStream << "Checksum Status: " << meta.checksum_status << "\n";
//
//            std::cout << "[Layer 5 - Session]\n";
//            std::cout << "Session ID: " << meta.session_id << "\n";
//            std::cout << "Session State: " << meta.session_state << "\n";
//            logStream << "[Layer 5 - Session]\n";
//            logStream << "Session ID: " << meta.session_id << "\n";
//            logStream << "Session State: " << meta.session_state << "\n";
//
//            // Layers 6-7: Presentation and Application
//            const u_char* app_data = transport_data + sizeof(udp_header);
//            int app_data_size = meta.payload_size - ip_hlen - sizeof(udp_header);
//            if (app_data_size > 0) {
//                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
//                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
//                meta.compression = "None";
//
//                std::cout << "[Layer 6 - Presentation]\n";
//                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
//                std::cout << "Compression: " << meta.compression << "\n";
//                logStream << "[Layer 6 - Presentation]\n";
//                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
//                logStream << "Compression: " << meta.compression << "\n";
//
//                std::cout << "[Layer 7 - Application]\n";
//                std::cout << "Protocol: " << meta.app_protocol << "\n";
//                std::cout << "Application Data: " << meta.app_data << "\n";
//                logStream << "[Layer 7 - Application]\n";
//                logStream << "Protocol: " << meta.app_protocol << "\n";
//                logStream << "Application Data: " << meta.app_data << "\n";
//            }
//        }
//        else if (ip_hdr->proto == IP_PROTO_ICMP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(icmp_header)) {
//            const icmp_header* icmp_hdr = reinterpret_cast<const icmp_header*>(transport_data);
//            meta.icmp_info = parseIcmp(icmp_hdr);
//            meta.checksum_status = "Not verified";
//            meta.session_id = "N/A (ICMP)";
//            meta.session_state = "Stateless";
//
//            std::cout << "[Layer 4 - Transport]\n";
//            std::cout << "Protocol: ICMP\n";
//            std::cout << "ICMP Info: " << meta.icmp_info << "\n";
//            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
//            logStream << "[Layer 4 - Transport]\n";
//            logStream << "Protocol: ICMP\n";
//            logStream << "ICMP Info: " << meta.icmp_info << "\n";
//            logStream << "Checksum Status: " << meta.checksum_status << "\n";
//
//            std::cout << "[Layer 5 - Session]\n";
//            std::cout << "Session ID: " << meta.session_id << "\n";
//            std::cout << "Session State: " << meta.session_state << "\n";
//            logStream << "[Layer 5 - Session]\n";
//            logStream << "Session ID: " << meta.session_id << "\n";
//            logStream << "Session State: " << meta.session_state << "\n";
//
//            std::cout << "[Layer 6 - Presentation]\n";
//            std::cout << "TLS Version: None\n";
//            std::cout << "Cipher Suite: None\n";
//            std::cout << "Compression: None\n";
//            logStream << "[Layer 6 - Presentation]\n";
//            logStream << "TLS Version: None\n";
//            logStream << "Cipher Suite: None\n";
//            logStream << "Compression: None\n";
//
//            std::cout << "[Layer 7 - Application]\n";
//            std::cout << "Protocol: ICMP\n";
//            std::cout << "Application Data: None\n";
//            logStream << "[Layer 7 - Application]\n";
//            logStream << "Protocol: ICMP\n";
//            logStream << "Application Data: None\n";
//        }
//
//        // Update flow stats
//        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
//        auto& stats = flow_stats[ip_pair];
//        stats.packet_count++;
//        stats.total_bytes += meta.payload_size;
//        if (stats.last_timestamp) {
//            double duration = timestamp - stats.last_timestamp;
//            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
//        }
//        stats.last_timestamp = timestamp;
//    }
//    else if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IPV6 && header->caplen >= sizeof(ethernet_header) + 40) {
//        const u_char* ipv6_data = next_layer;
//        u_char src_ipv6[16], dst_ipv6[16];
//        memcpy(src_ipv6, ipv6_data + 8, 16);
//        memcpy(dst_ipv6, ipv6_data + 24, 16);
//        meta.src_ip = ipv6ToString(src_ipv6);
//        meta.dst_ip = ipv6ToString(dst_ipv6);
//        meta.protocol = std::to_string(*(ipv6_data + 6));
//        meta.direction = (meta.src_ip.find("fe80::") == 0) ? "Outgoing" : "Incoming";
//
//        std::cout << "[Layer 3 - Network]\n";
//        std::cout << "Source IPv6: " << meta.src_ip << "\n";
//        std::cout << "Destination IPv6: " << meta.dst_ip << "\n";
//        std::cout << "Next Header: " << meta.protocol << "\n";
//        std::cout << "Direction: " << meta.direction << "\n";
//        logStream << "[Layer 3 - Network]\n";
//        logStream << "Source IPv6: " << meta.src_ip << "\n";
//        logStream << "Destination IPv6: " << meta.dst_ip << "\n";
//        logStream << "Next Header: " << meta.protocol << "\n";
//        logStream << "Direction: " << meta.direction << "\n";
//
//        uint8_t next_header = *(ipv6_data + 6);
//        int ipv6_hlen = 40;
//        const u_char* transport_data = ipv6_data + ipv6_hlen;
//        if (next_header == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ipv6_hlen + sizeof(udp_header)) {
//            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
//            meta.src_port = ntohs(udp_hdr->src_port);
//            meta.dst_port = ntohs(udp_hdr->dst_port);
//            meta.protocol = "UDP";
//            meta.checksum_status = "Not verified";
//            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
//            meta.session_state = "Connectionless";
//
//            std::cout << "[Layer 4 - Transport]\n";
//            std::cout << "Protocol: UDP\n";
//            std::cout << "Source Port: " << meta.src_port << "\n";
//            std::cout << "Destination Port: " << meta.dst_port << "\n";
//            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
//            logStream << "[Layer 4 - Transport]\n";
//            logStream << "Protocol: UDP\n";
//            logStream << "Source Port: " << meta.src_port << "\n";
//            logStream << "Destination Port: " << meta.dst_port << "\n";
//            logStream << "Checksum Status: " << meta.checksum_status << "\n";
//
//            std::cout << "[Layer 5 - Session]\n";
//            std::cout << "Session ID: " << meta.session_id << "\n";
//            std::cout << "Session State: " << meta.session_state << "\n";
//            logStream << "[Layer 5 - Session]\n";
//            logStream << "Session ID: " << meta.session_id << "\n";
//            logStream << "Session State: " << meta.session_state << "\n";
//
//            const u_char* app_data = transport_data + sizeof(udp_header);
//            int app_data_size = meta.payload_size - ipv6_hlen - sizeof(udp_header);
//            if (app_data_size > 0) {
//                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
//                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
//                meta.compression = "None";
//
//                std::cout << "[Layer 6 - Presentation]\n";
//                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
//                std::cout << "Compression: " << meta.compression << "\n";
//                logStream << "[Layer 6 - Presentation]\n";
//                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
//                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
//                logStream << "Compression: " << meta.compression << "\n";
//
//                std::cout << "[Layer 7 - Application]\n";
//                std::cout << "Protocol: " << meta.app_protocol << "\n";
//                std::cout << "Application Data: " << meta.app_data << "\n";
//                logStream << "[Layer 7 - Application]\n";
//                logStream << "Protocol: " << meta.app_protocol << "\n";
//                logStream << "Application Data: " << meta.app_data << "\n";
//            }
//        }
//
//        // Update flow stats
//        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
//        auto& stats = flow_stats[ip_pair];
//        stats.packet_count++;
//        stats.total_bytes += meta.payload_size;
//        if (stats.last_timestamp) {
//            double duration = timestamp - stats.last_timestamp;
//            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
//        }
//        stats.last_timestamp = timestamp;
//    }
//    else {
//        meta.protocol = "Non-IP";
//        meta.direction = "Unknown";
//        meta.session_id = "N/A";
//        meta.session_state = "N/A";
//        meta.app_protocol = "Unknown";
//        meta.app_data = "Non-IP packet";
//
//        std::cout << "[Layer 3 - Network]\n";
//        std::cout << "Non-IP packet, no IP addresses available\n";
//        std::cout << "Direction: " << meta.direction << "\n";
//        logStream << "[Layer 3 - Network]\n";
//        logStream << "Non-IP packet, no IP addresses available\n";
//        logStream << "Direction: " << meta.direction << "\n";
//
//        std::cout << "[Layer 4 - Transport]\n";
//        std::cout << "Protocol: None\n";
//        logStream << "[Layer 4 - Transport]\n";
//        logStream << "Protocol: None\n";
//
//        std::cout << "[Layer 5 - Session]\n";
//        std::cout << "Session ID: " << meta.session_id << "\n";
//        std::cout << "Session State: " << meta.session_state << "\n";
//        logStream << "[Layer 5 - Session]\n";
//        logStream << "Session ID: " << meta.session_id << "\n";
//        logStream << "Session State: " << meta.session_state << "\n";
//
//        std::cout << "[Layer 6 - Presentation]\n";
//        std::cout << "TLS Version: None\n";
//        std::cout << "Cipher Suite: None\n";
//        std::cout << "Compression: None\n";
//        logStream << "[Layer 6 - Presentation]\n";
//        logStream << "TLS Version: None\n";
//        logStream << "Cipher Suite: None\n";
//        logStream << "Compression: None\n";
//
//        std::cout << "[Layer 7 - Application]\n";
//        std::cout << "Protocol: " << meta.app_protocol << "\n";
//        std::cout << "Application Data: " << meta.app_data << "\n";
//        logStream << "[Layer 7 - Application]\n";
//        logStream << "Protocol: " << meta.app_protocol << "\n";
//        logStream << "Application Data: " << meta.app_data << "\n";
//    }
//
//    Logger::getInstance().logPacket(logStream.str());
//
//    // Log flow stats every 100 packets
//    if (packet_count % 100 == 0) {
//        std::ostringstream flowStream;
//        for (const auto& [pair, stats] : flow_stats) {
//            flowStream << "Flow: " << pair.first << " -> " << pair.second << "\n";
//            flowStream << "Packets: " << stats.packet_count << ", Bytes: "
//                << stats.total_bytes << ", Rate: " << stats.byte_rate << " B/s\n";
//        }
//        Logger::getInstance().logFlowStats(flowStream.str());
//    }
//
//    std::cout << std::dec;
//}
#define _CRT_SECURE_NO_WARNINGS
#include "PacketSniffer.h"
#include "Logger.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Define static members
double PacketSniffer::last_timestamp = 0;
std::map<std::pair<std::string, std::string>, FlowStats> PacketSniffer::flow_stats;
int PacketSniffer::packet_count = 0;

PacketSniffer::PacketSniffer(const std::string& deviceName) : deviceName(deviceName), handle(nullptr) {}

PacketSniffer::~PacketSniffer() {
    if (handle) {
        pcap_close(handle);
    }
}

void PacketSniffer::listInterfaces() {
    pcap_if_t* alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return;
    }

    interfaces.clear();
    int i = 0;
    for (pcap_if_t* d = alldevs; d; d = d->next) {
        interfaces.push_back(d);
        std::cout << i++ << ": " << (d->description ? d->description : "No description") << std::endl;
    }
    pcap_freealldevs(alldevs);
}

void PacketSniffer::startSniffing(int index) {
    if (index < 0 || index >= interfaces.size()) {
        std::cerr << "Invalid interface index." << std::endl;
        return;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(interfaces[index]->name, 65535, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Error opening device: " << errbuf << std::endl;
        return;
    }

    if (pcap_set_promisc(handle, 1) != 0) {
        std::cerr << "Error setting promiscuous mode: " << pcap_geterr(handle) << std::endl;
    }

    std::cout << "[*] Starting packet capture on " << interfaces[index]->name << "...\n" << std::endl;
    int result = pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char*>(this));
    if (result == -1) {
        std::cerr << "pcap_loop error: " << pcap_geterr(handle) << std::endl;
    }
    else if (result == -2) {
        std::cout << "pcap_loop terminated by user break." << std::endl;
    }

    pcap_close(handle);
}

std::string PacketSniffer::macToString(const u_char* mac) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
        oss << std::setw(2) << (int)mac[i];
        if (i < 5) oss << ":";
    }
    return oss.str();
}

std::string PacketSniffer::ipToString(const in_addr& addr) {
    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN)) {
        return std::string(ip_str);
    }
    return "Invalid IPv4";
}

std::string PacketSniffer::ipv6ToString(const u_char* addr) {
    char ip_str[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, addr, ip_str, INET6_ADDRSTRLEN)) {
        return std::string(ip_str);
    }
    return "Invalid IPv6";
}

std::string PacketSniffer::getTcpFlags(const tcp_header* tcp_hdr) {
    std::string flags;
    if (tcp_hdr->flags & 0x01) flags += "FIN ";
    if (tcp_hdr->flags & 0x02) flags += "SYN ";
    if (tcp_hdr->flags & 0x04) flags += "RST ";
    if (tcp_hdr->flags & 0x08) flags += "PSH ";
    if (tcp_hdr->flags & 0x10) flags += "ACK ";
    if (tcp_hdr->flags & 0x20) flags += "URG ";
    return flags.empty() ? "None" : flags;
}

void PacketSniffer::logPayload(const u_char* payload, int payload_size, PacketMetadata& meta) {
    std::ostringstream hex_ascii;
    for (int i = 0; i < payload_size; i += 16) {
        hex_ascii << std::setw(4) << std::setfill('0') << i << "  ";
        for (int j = 0; j < 16; ++j) {
            if (i + j < payload_size) {
                unsigned char c = static_cast<unsigned char>(payload[i + j]);
                hex_ascii << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
            }
            else {
                hex_ascii << "   ";
            }
        }
        hex_ascii << " ";
        for (int j = 0; j < 16; ++j) {
            if (i + j < payload_size) {
                unsigned char c = static_cast<unsigned char>(payload[i + j]);
                hex_ascii << (std::isprint(c) ? static_cast<char>(c) : '.');
            }
            else {
                hex_ascii << " ";
            }
        }
        hex_ascii << "\n";
    }
    meta.payload_hex_ascii = hex_ascii.str();
}

std::string PacketSniffer::parseIpOptions(const u_char* options, int options_len) {
    std::ostringstream oss;
    int pos = 0;
    while (pos < options_len) {
        uint8_t opt_code = options[pos];
        if (opt_code == 0) { // End of options
            oss << "End of Options";
            break;
        }
        if (opt_code == 1) { // No Operation
            oss << "NOP ";
            pos++;
            continue;
        }
        if (pos + 1 >= options_len) break;
        uint8_t opt_len = options[pos + 1];
        if (opt_len < 2 || pos + opt_len > options_len) break;
        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len << " ";
        pos += opt_len;
    }
    return oss.str();
}

std::string PacketSniffer::parseTcpOptions(const u_char* options, int options_len) {
    std::ostringstream oss;
    int pos = 0;
    while (pos < options_len) {
        uint8_t opt_code = options[pos];
        if (opt_code == 0) { // End of options
            oss << "End of Options";
            break;
        }
        if (opt_code == 1) { // No Operation
            oss << "NOP ";
            pos++;
            continue;
        }
        if (pos + 1 >= options_len) break;
        uint8_t opt_len = options[pos + 1];
        if (opt_len < 2 || pos + opt_len > options_len) break;
        oss << "Option Code: " << (int)opt_code << ", Length: " << (int)opt_len;
        if (opt_code == 2) oss << " (MSS)";
        else if (opt_code == 3) oss << " (Window Scale)";
        else if (opt_code == 4) oss << " (SACK Permitted)";
        else if (opt_code == 8) oss << " (Timestamps)";
        oss << " ";
        pos += opt_len;
    }
    return oss.str();
}

std::string PacketSniffer::parseApplicationPayload(const u_char* payload, int payload_size, const std::string& protocol, uint16_t src_port, uint16_t dst_port, PacketMetadata& meta) {
    if (payload_size <= 0) return "No application data";

    // Try protocol-specific parsing
    if (protocol == "TCP" || protocol == "UDP") {
        if (src_port == 80 || dst_port == 80 || src_port == 8080 || dst_port == 8080) {
            meta.app_protocol = "HTTP";
            meta.http_info = parseHttp(payload, payload_size);
            if (!meta.http_info.empty()) return meta.http_info;
        }
        else if (src_port == 443 || dst_port == 443) {
            meta.app_protocol = "TLS";
            meta.tls_version = parseTlsVersion(payload, payload_size);
            if (!meta.tls_version.empty()) return "TLS Version: " + meta.tls_version;
        }
        else if (src_port == 25 || dst_port == 25 || src_port == 587 || dst_port == 587) {
            meta.app_protocol = "SMTP";
            meta.smtp_info = parseSmtp(payload, payload_size);
            if (!meta.smtp_info.empty()) return meta.smtp_info;
        }
        else if (src_port == 143 || dst_port == 143 || src_port == 993 || dst_port == 993) {
            meta.app_protocol = "IMAP";
            meta.imap_info = parseImap(payload, payload_size);
            if (!meta.imap_info.empty()) return meta.imap_info;
        }
        else if (src_port == 110 || dst_port == 110 || src_port == 995 || dst_port == 995) {
            meta.app_protocol = "POP3";
            meta.pop3_info = parsePop3(payload, payload_size);
            if (!meta.pop3_info.empty()) return meta.pop3_info;
        }
        else if (src_port == 5060 || dst_port == 5060 || src_port == 5061 || dst_port == 5061) {
            meta.app_protocol = "SIP";
            meta.sip_info = parseSip(payload, payload_size);
            if (!meta.sip_info.empty()) return meta.sip_info;
        }
        else if ((src_port >= 10000 && src_port <= 65535 && dst_port >= 10000 && dst_port <= 65535) && protocol == "UDP") {
            meta.app_protocol = "RTP";
            meta.rtp_info = parseRtp(payload, payload_size);
            if (!meta.rtp_info.empty()) return meta.rtp_info;
        }
        else if (src_port == 21 || dst_port == 21) {
            meta.app_protocol = "FTP";
            meta.ftp_info = parseFtp(payload, payload_size);
            if (!meta.ftp_info.empty()) return meta.ftp_info;
        }
        else if (src_port == 53 || dst_port == 53 || src_port == 5353 || dst_port == 5353) {
            meta.app_protocol = "DNS";
            meta.dns_query = parseDnsQuery(payload, payload_size);
            if (!meta.dns_query.empty()) return (src_port == 5353 || dst_port == 5353) ? "mDNS Query: " + meta.dns_query : "DNS Query: " + meta.dns_query;
        }
    }

    // Fallback: Generic payload parsing for unknown protocols
    meta.app_protocol = "Unknown";
    std::string readable;
    for (int i = 0; i < payload_size; ++i) {
        unsigned char c = payload[i];
        if (std::isprint(c)) {
            readable += static_cast<char>(c);
        }
        else if (c == '\r' || c == '\n') {
            readable += c;
        }
    }
    meta.app_data = readable.empty() ? "No readable application data" : readable;
    return meta.app_data;
}

std::string PacketSniffer::parseDnsQuery(const u_char* payload, int payload_size) {
    if (payload_size < 12) return "";
    std::string query;
    int pos = 12;
    while (pos < payload_size && payload[pos]) {
        int seg_len = payload[pos++];
        if (pos + seg_len >= payload_size) break;
        for (int i = 0; i < seg_len; ++i) {
            query += (char)payload[pos++];
        }
        query += ".";
    }
    if (!query.empty()) query.pop_back();
    return query;
}

std::string PacketSniffer::parseTlsVersion(const u_char* payload, int payload_size) {
    if (payload_size < 5 || payload[0] != 0x16) return "";
    if (payload[1] == 0x03 && payload[2] == 0x03) return "TLS 1.2";
    if (payload[1] == 0x03 && payload[2] == 0x04) return "TLS 1.3";
    return "";
}

std::string PacketSniffer::parseHttp(const u_char* payload, int payload_size) {
    std::string http_data(reinterpret_cast<const char*>(payload), payload_size);
    std::size_t pos = http_data.find("\r\n");
    if (pos != std::string::npos) {
        std::string first_line = http_data.substr(0, pos);
        if (first_line.find("HTTP/") != std::string::npos ||
            first_line.find("GET ") != std::string::npos ||
            first_line.find("POST ") != std::string::npos) {
            return first_line;
        }
    }
    return "";
}

std::string PacketSniffer::parseSmtp(const u_char* payload, int payload_size) {
    std::string smtp_data(reinterpret_cast<const char*>(payload), payload_size);
    std::ostringstream result;
    std::size_t pos = 0;
    if (smtp_data.find("HELO ") == 0 || smtp_data.find("EHLO ") == 0 ||
        smtp_data.find("MAIL FROM:") == 0 || smtp_data.find("RCPT TO:") == 0) {
        pos = smtp_data.find("\r\n");
        if (pos != std::string::npos) {
            result << smtp_data.substr(0, pos);
        }
        else {
            result << smtp_data;
        }
    }
    else if (smtp_data.find("DATA\r\n") == 0) {
        pos = smtp_data.find("\r\n", 6);
        if (pos != std::string::npos) {
            std::string email_content = smtp_data.substr(pos + 2);
            std::size_t header_end = email_content.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                std::string headers = email_content.substr(0, header_end);
                std::istringstream header_stream(headers);
                std::string line;
                while (std::getline(header_stream, line) && !line.empty()) {
                    if (line.find("Subject: ") == 0 || line.find("From: ") == 0 || line.find("To: ") == 0) {
                        result << "\n" << line;
                    }
                }
                std::string body = email_content.substr(header_end + 4, 100);
                if (!body.empty()) {
                    result << "\nBody: " << body;
                }
            }
        }
    }
    return result.str();
}

std::string PacketSniffer::parseImap(const u_char* payload, int payload_size) {
    std::string imap_data(reinterpret_cast<const char*>(payload), payload_size);
    std::size_t pos = imap_data.find("\r\n");
    if (pos != std::string::npos) {
        std::string command = imap_data.substr(0, pos);
        if (command.find("LOGIN ") == 0 || command.find("FETCH ") == 0 ||
            command.find("SELECT ") == 0 || command.find("LIST ") == 0) {
            return command;
        }
        else if (imap_data.find("* OK ") == 0 || imap_data.find("* FLAGS ") == 0 ||
            imap_data.find("* SEARCH ") == 0) {
            return imap_data.substr(0, pos);
        }
    }
    return "";
}

std::string PacketSniffer::parsePop3(const u_char* payload, int payload_size) {
    std::string pop3_data(reinterpret_cast<const char*>(payload), payload_size);
    std::size_t pos = pop3_data.find("\r\n");
    if (pos != std::string::npos) {
        std::string command = pop3_data.substr(0, pos);
        if (command.find("USER ") == 0 || command.find("PASS ") == 0 ||
            command.find("RETR ") == 0 || command.find("LIST ") == 0) {
            return command;
        }
        else if (pop3_data.find("+OK ") == 0 || pop3_data.find("-ERR ") == 0) {
            return pop3_data.substr(0, pos);
        }
    }
    return "";
}

std::string PacketSniffer::parseSip(const u_char* payload, int payload_size) {
    std::string sip_data(reinterpret_cast<const char*>(payload), payload_size);
    std::size_t pos = sip_data.find("\r\n");
    if (pos != std::string::npos) {
        std::string first_line = sip_data.substr(0, pos);
        if (first_line.find("INVITE ") == 0 || first_line.find("REGISTER ") == 0 ||
            first_line.find("BYE ") == 0 || first_line.find("ACK ") == 0 ||
            first_line.find("SIP/2.0 ") == 0) {
            std::ostringstream result;
            result << first_line;
            std::size_t call_id_pos = sip_data.find("Call-ID: ");
            if (call_id_pos != std::string::npos) {
                std::size_t end_pos = sip_data.find("\r\n", call_id_pos);
                if (end_pos != std::string::npos) {
                    result << "\n" << sip_data.substr(call_id_pos, end_pos - call_id_pos);
                }
            }
            return result.str();
        }
    }
    return "";
}

std::string PacketSniffer::parseRtp(const u_char* payload, int payload_size) {
    if (payload_size < 12) return "";
    std::ostringstream result;
    uint8_t version = (payload[0] >> 6) & 0x03;
    uint8_t payload_type = payload[1] & 0x7F;
    uint16_t sequence = ntohs(*reinterpret_cast<const uint16_t*>(&payload[2]));
    uint32_t timestamp = ntohl(*reinterpret_cast<const uint32_t*>(&payload[4]));
    uint32_t ssrc = ntohl(*reinterpret_cast<const uint32_t*>(&payload[8]));

    if (version != 2) return "";

    result << "Version: " << (int)version << ", Payload Type: " << (int)payload_type
        << ", Sequence: " << sequence << ", Timestamp: " << timestamp
        << ", SSRC: " << ssrc;

    std::string codec;
    switch (payload_type) {
    case 0: codec = "PCMU"; break;
    case 8: codec = "PCMA"; break;
    case 18: codec = "G729"; break;
    case 34: codec = "H263"; break;
    default: codec = "Unknown"; break;
    }
    result << ", Codec: " << codec;
    return result.str();
}

std::string PacketSniffer::parseFtp(const u_char* payload, int payload_size) {
    std::string ftp_data(reinterpret_cast<const char*>(payload), payload_size);
    if (ftp_data.find("USER ") == 0 || ftp_data.find("PASS ") == 0 ||
        ftp_data.find("RETR ") == 0 || ftp_data.find("STOR ") == 0) {
        std::size_t pos = ftp_data.find("\r\n");
        if (pos != std::string::npos) {
            return ftp_data.substr(0, pos);
        }
        return ftp_data;
    }
    return "";
}

std::string PacketSniffer::parseIcmp(const icmp_header* icmp_hdr) {
    std::ostringstream oss;
    oss << "Type: " << (int)icmp_hdr->type << ", Code: " << (int)icmp_hdr->code;
    if (icmp_hdr->type == 8) oss << " (Echo Request)";
    else if (icmp_hdr->type == 0) oss << " (Echo Reply)";
    return oss.str();
}

void PacketSniffer::packetHandler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
    PacketSniffer* sniffer = reinterpret_cast<PacketSniffer*>(param);
    sniffer->printPacketDetails(header, pkt_data);
}

void PacketSniffer::printPacketDetails(const struct pcap_pkthdr* header, const u_char* pkt_data) {
    PacketMetadata meta;
    double timestamp = header->ts.tv_sec + header->ts.tv_usec / 1000000.0;
    meta.timestamp = currentTimestamp(timestamp);
    meta.inter_arrival = 0; // Initialize to 0, will be updated for IP packets
    meta.captured_length = header->caplen;
    meta.total_length = header->len;
    last_timestamp = timestamp;

    // Layer 1: Physical
    meta.interface_name = interfaces.empty() ? "Unknown" : interfaces[0]->name;
    meta.interface_description = interfaces.empty() ? "Unknown" : (interfaces[0]->description ? interfaces[0]->description : "No description");

    std::ostringstream logStream;
    logStream << "===== Packet #" << ++packet_count << " =====\n";
    logStream << "[Layer 1 - Physical]\n";
    logStream << "Interface Name: " << meta.interface_name << "\n";
    logStream << "Interface Description: " << meta.interface_description << "\n";
    logStream << "[Timing Information]\n";
    logStream << "Timestamp: " << meta.timestamp << "\n";
    logStream << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
    logStream << "Captured Length: " << meta.captured_length << " bytes\n";
    logStream << "Total Length: " << meta.total_length << " bytes\n";

    std::cout << "\n===== Packet #" << packet_count << " =====\n";
    std::cout << "[Layer 1 - Physical]\n";
    std::cout << "Interface Name: " << meta.interface_name << "\n";
    std::cout << "Interface Description: " << meta.interface_description << "\n";
    std::cout << "[Timing Information]\n";
    std::cout << "Timestamp: " << meta.timestamp << "\n";
    std::cout << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
    std::cout << "Captured Length: " << meta.captured_length << " bytes\n";
    std::cout << "Total Length: " << meta.total_length << " bytes\n";

    // Layer 2: Data Link
    if (header->caplen < sizeof(ethernet_header)) {
        meta.eth_type = "Unknown (Malformed)";
        std::cout << "[Layer 2 - Data Link]\n";
        std::cout << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
        logStream << "[Layer 2 - Data Link]\n";
        logStream << "Ethernet Type: " << meta.eth_type << " (Packet too short for Ethernet header)\n";
        Logger::getInstance().logPacket(logStream.str());
        return;
    }

    const ethernet_header* eth_hdr = reinterpret_cast<const ethernet_header*>(pkt_data);
    meta.src_mac = macToString(eth_hdr->src_mac);
    meta.dst_mac = macToString(eth_hdr->dest_mac);
    meta.eth_type = "0x" + std::to_string(ntohs(eth_hdr->ether_type));

    const u_char* next_layer = pkt_data + sizeof(ethernet_header);
    meta.payload_size = header->caplen - sizeof(ethernet_header);

    // Check for VLAN tag
    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_VLAN) {
        uint16_t vlan_id = ntohs(*(uint16_t*)(next_layer)) & 0x0FFF;
        meta.vlan_tag = "VLAN ID: " + std::to_string(vlan_id);
        meta.eth_type = "0x" + std::to_string(ntohs(*(uint16_t*)(next_layer + 2)));
        next_layer += 4;
        meta.payload_size -= 4;
    }

    std::cout << "[Layer 2 - Data Link]\n";
    std::cout << "Source MAC: " << meta.src_mac << "\n";
    std::cout << "Destination MAC: " << meta.dst_mac << "\n";
    std::cout << "Ethernet Type: " << meta.eth_type << "\n";
    std::cout << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
    logStream << "[Layer 2 - Data Link]\n";
    logStream << "Source MAC: " << meta.src_mac << "\n";
    logStream << "Destination MAC: " << meta.dst_mac << "\n";
    logStream << "Ethernet Type: " << meta.eth_type << "\n";
    logStream << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";

    // Payload logging
    if (meta.payload_size > 0) {
        logPayload(next_layer, meta.payload_size, meta);
        std::cout << "[Payload]\n";
        std::cout << "Payload Size: " << meta.payload_size << " bytes\n";
        std::cout << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
        logStream << "[Payload]\n";
        logStream << "Payload Size: " << meta.payload_size << " bytes\n";
        logStream << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;
    }
    else {
        std::cout << "[Payload]]]n";
        std::cout << "No payload data\n";
        logStream << "[Payload]\n";
        logStream << "No payload data\n";
    }

    // Layer 3: Network
    if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IP && header->caplen >= sizeof(ethernet_header) + sizeof(ip_header)) {
        const ip_header* ip_hdr = reinterpret_cast<const ip_header*>(next_layer);
        int ip_hlen = (ip_hdr->ver_ihl & 0x0F) * 4;
        meta.src_ip = ipToString(ip_hdr->src_addr);
        meta.dst_ip = ipToString(ip_hdr->dst_addr);
        meta.protocol = (ip_hdr->proto == IP_PROTO_TCP) ? "TCP" :
            (ip_hdr->proto == IP_PROTO_UDP) ? "UDP" :
            (ip_hdr->proto == IP_PROTO_ICMP) ? "ICMP" :
            std::to_string(ip_hdr->proto);
        meta.direction = (meta.src_ip.find("192.168.") == 0 || meta.src_ip.find("10.") == 0) ? "Outgoing" : "Incoming";

        // Calculate flow-specific delta time
        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
        auto& stats = flow_stats[ip_pair];
        if (stats.last_timestamp > 0) {
            meta.inter_arrival = timestamp - stats.last_timestamp;
        }
        stats.last_timestamp = timestamp;

        // Update logStream with correct delta time
        logStream.str(""); // Clear logStream
        logStream << "===== Packet #" << packet_count << " =====\n";
        logStream << "[Layer 1 - Physical]\n";
        logStream << "Interface Name: " << meta.interface_name << "\n";
        logStream << "Interface Description: " << meta.interface_description << "\n";
        logStream << "[Timing Information]\n";
        logStream << "Timestamp: " << meta.timestamp << "\n";
        logStream << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
        logStream << "Captured Length: " << meta.captured_length << " bytes\n";
        logStream << "Total Length: " << meta.total_length << " bytes\n";
        logStream << "[Layer 2 - Data Link]\n";
        logStream << "Source MAC: " << meta.src_mac << "\n";
        logStream << "Destination MAC: " << meta.dst_mac << "\n";
        logStream << "Ethernet Type: " << meta.eth_type << "\n";
        logStream << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
        logStream << "[Payload]\n";
        logStream << "Payload Size: " << meta.payload_size << " bytes\n";
        logStream << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;

        // Update console output with correct delta time
        std::cout << "\n===== Packet #" << packet_count << " =====\n";
        std::cout << "[Layer 1 - Physical]\n";
        std::cout << "Interface Name: " << meta.interface_name << "\n";
        std::cout << "Interface Description: " << meta.interface_description << "\n";
        std::cout << "[Timing Information]\n";
        std::cout << "Timestamp: " << meta.timestamp << "\n";
        std::cout << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
        std::cout << "Captured Length: " << meta.captured_length << " bytes\n";
        std::cout << "Total Length: " << meta.total_length << " bytes\n";
        std::cout << "[Layer 2 - Data Link]\n";
        std::cout << "Source MAC: " << meta.src_mac << "\n";
        std::cout << "Destination MAC: " << meta.dst_mac << "\n";
        std::cout << "Ethernet Type: " << meta.eth_type << "\n";
        std::cout << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
        std::cout << "[Payload]\n";
        std::cout << "Payload Size: " << meta.payload_size << " bytes\n";
        std::cout << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;

        // Parse IP options
        if (ip_hlen > sizeof(ip_header)) {
            meta.ip_options = parseIpOptions(next_layer + sizeof(ip_header), ip_hlen - sizeof(ip_header));
        }

        std::cout << "[Layer 3 - Network]\n";
        std::cout << "Source IP: " << meta.src_ip << "\n";
        std::cout << "Destination IP: " << meta.dst_ip << "\n";
        std::cout << "Protocol: " << meta.protocol << "\n";
        std::cout << "Direction: " << meta.direction << "\n";
        std::cout << "IP Header Length: " << ip_hlen << " bytes\n";
        std::cout << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";
        logStream << "[Layer 3 - Network]\n";
        logStream << "Source IP: " << meta.src_ip << "\n";
        logStream << "Destination IP: " << meta.dst_ip << "\n";
        logStream << "Protocol: " << meta.protocol << "\n";
        logStream << "Direction: " << meta.direction << "\n";
        logStream << "IP Header Length: " << ip_hlen << " bytes\n";
        logStream << "IP Options: " << (meta.ip_options.empty() ? "None" : meta.ip_options) << "\n";

        // Layer 4: Transport
        const u_char* transport_data = next_layer + ip_hlen;
        if (ip_hdr->proto == IP_PROTO_TCP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(tcp_header)) {
            const tcp_header* tcp_hdr = reinterpret_cast<const tcp_header*>(transport_data);
            meta.src_port = ntohs(tcp_hdr->src_port);
            meta.dst_port = ntohs(tcp_hdr->dst_port);
            meta.tcp_flags = getTcpFlags(tcp_hdr);
            int tcp_hlen = ((tcp_hdr->offset_res >> 4) & 0x0F) * 4;
            meta.checksum_status = "Not verified";

            // Parse TCP options
            if (tcp_hlen > sizeof(tcp_header)) {
                meta.transport_options = parseTcpOptions(transport_data + sizeof(tcp_header), tcp_hlen - sizeof(tcp_header));
            }

            // Layer 5: Session
            meta.session_id = std::to_string(ntohl(tcp_hdr->seq)) + ":" + std::to_string(ntohl(tcp_hdr->ack_seq));
            meta.session_state = (tcp_hdr->flags & 0x02) ? "SYN Sent" :
                (tcp_hdr->flags & 0x10 && !(tcp_hdr->flags & 0x01)) ? "Established" :
                (tcp_hdr->flags & 0x01) ? "FIN Sent" : "Unknown";

            std::cout << "[Layer 4 - Transport]\n";
            std::cout << "Protocol: TCP\n";
            std::cout << "Source Port: " << meta.src_port << "\n";
            std::cout << "Destination Port: " << meta.dst_port << "\n";
            std::cout << "TCP Flags: " << meta.tcp_flags << "\n";
            std::cout << "Header Length: " << tcp_hlen << " bytes\n";
            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
            std::cout << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";
            logStream << "[Layer 4 - Transport]\n";
            logStream << "Protocol: TCP\n";
            logStream << "Source Port: " << meta.src_port << "\n";
            logStream << "Destination Port: " << meta.dst_port << "\n";
            logStream << "TCP Flags: " << meta.tcp_flags << "\n";
            logStream << "Header Length: " << tcp_hlen << " bytes\n";
            logStream << "Checksum Status: " << meta.checksum_status << "\n";
            logStream << "TCP Options: " << (meta.transport_options.empty() ? "None" : meta.transport_options) << "\n";

            std::cout << "[Layer 5 - Session]\n";
            std::cout << "Session ID: " << meta.session_id << "\n";
            std::cout << "Session State: " << meta.session_state << "\n";
            logStream << "[Layer 5 - Session]\n";
            logStream << "Session ID: " << meta.session_id << "\n";
            logStream << "Session State: " << meta.session_state << "\n";

            // Layers 6-7: Presentation and Application
            const u_char* app_data = transport_data + tcp_hlen;
            int app_data_size = meta.payload_size - ip_hlen - tcp_hlen;
            if (app_data_size > 0) {
                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
                meta.compression = "None";

                std::cout << "[Layer 6 - Presentation]\n";
                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
                std::cout << "Compression: " << meta.compression << "\n";
                logStream << "[Layer 6 - Presentation]\n";
                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
                logStream << "Compression: " << meta.compression << "\n";

                std::cout << "[Layer 7 - Application]\n";
                std::cout << "Protocol: " << meta.app_protocol << "\n";
                std::cout << "Application Data: " << meta.app_data << "\n";
                logStream << "[Layer 7 - Application]\n";
                logStream << "Protocol: " << meta.app_protocol << "\n";
                logStream << "Application Data: " << meta.app_data << "\n";
            }
        }
        else if (ip_hdr->proto == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(udp_header)) {
            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
            meta.src_port = ntohs(udp_hdr->src_port);
            meta.dst_port = ntohs(udp_hdr->dst_port);
            meta.checksum_status = "Not verified";

            // Layer 5: Session (UDP is connectionless, but protocols like SIP may have sessions)
            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
            meta.session_state = "Connectionless";

            std::cout << "[Layer 4 - Transport]\n";
            std::cout << "Protocol: UDP\n";
            std::cout << "Source Port: " << meta.src_port << "\n";
            std::cout << "Destination Port: " << meta.dst_port << "\n";
            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
            logStream << "[Layer 4 - Transport]\n";
            logStream << "Protocol: UDP\n";
            logStream << "Source Port: " << meta.src_port << "\n";
            logStream << "Destination Port: " << meta.dst_port << "\n";
            logStream << "Checksum Status: " << meta.checksum_status << "\n";

            std::cout << "[Layer 5 - Session]\n";
            std::cout << "Session ID: " << meta.session_id << "\n";
            std::cout << "Session State: " << meta.session_state << "\n";
            logStream << "[Layer 5 - Session]\n";
            logStream << "Session ID: " << meta.session_id << "\n";
            logStream << "Session State: " << meta.session_state << "\n";

            // Layers 6-7: Presentation and Application
            const u_char* app_data = transport_data + sizeof(udp_header);
            int app_data_size = meta.payload_size - ip_hlen - sizeof(udp_header);
            if (app_data_size > 0) {
                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
                meta.compression = "None";

                std::cout << "[Layer 6 - Presentation]\n";
                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
                std::cout << "Compression: " << meta.compression << "\n";
                logStream << "[Layer 6 - Presentation]\n";
                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
                logStream << "Compression: " << meta.compression << "\n";

                std::cout << "[Layer 7 - Application]\n";
                std::cout << "Protocol: " << meta.app_protocol << "\n";
                std::cout << "Application Data: " << meta.app_data << "\n";
                logStream << "[Layer 7 - Application]\n";
                logStream << "Protocol: " << meta.app_protocol << "\n";
                logStream << "Application Data: " << meta.app_data << "\n";
            }
        }
        else if (ip_hdr->proto == IP_PROTO_ICMP && header->caplen >= sizeof(ethernet_header) + ip_hlen + sizeof(icmp_header)) {
            const icmp_header* icmp_hdr = reinterpret_cast<const icmp_header*>(transport_data);
            meta.icmp_info = parseIcmp(icmp_hdr);
            meta.checksum_status = "Not verified";
            meta.session_id = "N/A (ICMP)";
            meta.session_state = "Stateless";

            std::cout << "[Layer 4 - Transport]\n";
            std::cout << "Protocol: ICMP\n";
            std::cout << "ICMP Info: " << meta.icmp_info << "\n";
            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
            logStream << "[Layer 4 - Transport]\n";
            logStream << "Protocol: ICMP\n";
            logStream << "ICMP Info: " << meta.icmp_info << "\n";
            logStream << "Checksum Status: " << meta.checksum_status << "\n";

            std::cout << "[Layer 5 - Session]\n";
            std::cout << "Session ID: " << meta.session_id << "\n";
            std::cout << "Session State: " << meta.session_state << "\n";
            logStream << "[Layer 5 - Session]\n";
            logStream << "Session ID: " << meta.session_id << "\n";
            logStream << "Session State: " << meta.session_state << "\n";

            std::cout << "[Layer 6 - Presentation]\n";
            std::cout << "TLS Version: None\n";
            std::cout << "Cipher Suite: None\n";
            std::cout << "Compression: None\n";
            logStream << "[Layer 6 - Presentation]\n";
            logStream << "TLS Version: None\n";
            logStream << "Cipher Suite: None\n";
            logStream << "Compression: None\n";

            std::cout << "[Layer 7 - Application]\n";
            std::cout << "Protocol: ICMP\n";
            std::cout << "Application Data: None\n";
            logStream << "[Layer 7 - Application]\n";
            logStream << "Protocol: ICMP\n";
            logStream << "Application Data: None\n";
        }

        // Update flow stats
        stats.packet_count++;
        stats.total_bytes += meta.payload_size;
        if (stats.last_timestamp) {
            double duration = timestamp - stats.last_timestamp;
            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
        }
    }
    else if (ntohs(eth_hdr->ether_type) == ETHER_TYPE_IPV6 && header->caplen >= sizeof(ethernet_header) + 40) {
        const u_char* ipv6_data = next_layer;
        u_char src_ipv6[16], dst_ipv6[16];
        memcpy(src_ipv6, ipv6_data + 8, 16);
        memcpy(dst_ipv6, ipv6_data + 24, 16);
        meta.src_ip = ipv6ToString(src_ipv6);
        meta.dst_ip = ipv6ToString(dst_ipv6);
        meta.protocol = std::to_string(*(ipv6_data + 6));
        meta.direction = (meta.src_ip.find("fe80::") == 0) ? "Outgoing" : "Incoming";

        // Calculate flow-specific delta time
        auto ip_pair = std::make_pair(meta.src_ip, meta.dst_ip);
        auto& stats = flow_stats[ip_pair];
        if (stats.last_timestamp > 0) {
            meta.inter_arrival = timestamp - stats.last_timestamp;
        }
        stats.last_timestamp = timestamp;

        // Update logStream with correct delta time
        logStream.str(""); // Clear logStream
        logStream << "===== Packet #" << packet_count << " =====\n";
        logStream << "[Layer 1 - Physical]\n";
        logStream << "Interface Name: " << meta.interface_name << "\n";
        logStream << "Interface Description: " << meta.interface_description << "\n";
        logStream << "[Timing Information]\n";
        logStream << "Timestamp: " << meta.timestamp << "\n";
        logStream << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
        logStream << "Captured Length: " << meta.captured_length << " bytes\n";
        logStream << "Total Length: " << meta.total_length << " bytes\n";
        logStream << "[Layer 2 - Data Link]\n";
        logStream << "Source MAC: " << meta.src_mac << "\n";
        logStream << "Destination MAC: " << meta.dst_mac << "\n";
        logStream << "Ethernet Type: " << meta.eth_type << "\n";
        logStream << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
        logStream << "[Payload]\n";
        logStream << "Payload Size: " << meta.payload_size << " bytes\n";
        logStream << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;

        // Update console output with correct delta time
        std::cout << "\n===== Packet #" << packet_count << " =====\n";
        std::cout << "[Layer 1 - Physical]\n";
        std::cout << "Interface Name: " << meta.interface_name << "\n";
        std::cout << "Interface Description: " << meta.interface_description << "\n";
        std::cout << "[Timing Information]\n";
        std::cout << "Timestamp: " << meta.timestamp << "\n";
        std::cout << "Delta Time: " << std::fixed << std::setprecision(6) << meta.inter_arrival << " seconds\n";
        std::cout << "Captured Length: " << meta.captured_length << " bytes\n";
        std::cout << "Total Length: " << meta.total_length << " bytes\n";
        std::cout << "[Layer 2 - Data Link]\n";
        std::cout << "Source MAC: " << meta.src_mac << "\n";
        std::cout << "Destination MAC: " << meta.dst_mac << "\n";
        std::cout << "Ethernet Type: " << meta.eth_type << "\n";
        std::cout << "VLAN Tag: " << (meta.vlan_tag.empty() ? "None" : meta.vlan_tag) << "\n";
        std::cout << "[Payload]\n";
        std::cout << "Payload Size: " << meta.payload_size << " bytes\n";
        std::cout << "Payload (Hex + ASCII):\n" << meta.payload_hex_ascii;

        std::cout << "[Layer 3 - Network]\n";
        std::cout << "Source IPv6: " << meta.src_ip << "\n";
        std::cout << "Destination IPv6: " << meta.dst_ip << "\n";
        std::cout << "Next Header: " << meta.protocol << "\n";
        std::cout << "Direction: " << meta.direction << "\n";
        logStream << "[Layer 3 - Network]\n";
        logStream << "Source IPv6: " << meta.src_ip << "\n";
        logStream << "Destination IPv6: " << meta.dst_ip << "\n";
        logStream << "Next Header: " << meta.protocol << "\n";
        logStream << "Direction: " << meta.direction << "\n";

        uint8_t next_header = *(ipv6_data + 6);
        int ipv6_hlen = 40;
        const u_char* transport_data = ipv6_data + ipv6_hlen;
        if (next_header == IP_PROTO_UDP && header->caplen >= sizeof(ethernet_header) + ipv6_hlen + sizeof(udp_header)) {
            const udp_header* udp_hdr = reinterpret_cast<const udp_header*>(transport_data);
            meta.src_port = ntohs(udp_hdr->src_port);
            meta.dst_port = ntohs(udp_hdr->dst_port);
            meta.protocol = "UDP";
            meta.checksum_status = "Not verified";
            meta.session_id = meta.src_ip + ":" + std::to_string(meta.src_port) + "->" + meta.dst_ip + ":" + std::to_string(meta.dst_port);
            meta.session_state = "Connectionless";

            std::cout << "[Layer 4 - Transport]\n";
            std::cout << "Protocol: UDP\n";
            std::cout << "Source Port: " << meta.src_port << "\n";
            std::cout << "Destination Port: " << meta.dst_port << "\n";
            std::cout << "Checksum Status: " << meta.checksum_status << "\n";
            logStream << "[Layer 4 - Transport]\n";
            logStream << "Protocol: UDP\n";
            logStream << "Source Port: " << meta.src_port << "\n";
            logStream << "Destination Port: " << meta.dst_port << "\n";
            logStream << "Checksum Status: " << meta.checksum_status << "\n";

            std::cout << "[Layer 5 - Session]\n";
            std::cout << "Session ID: " << meta.session_id << "\n";
            std::cout << "Session State: " << meta.session_state << "\n";
            logStream << "[Layer 5 - Session]\n";
            logStream << "Session ID: " << meta.session_id << "\n";
            logStream << "Session State: " << meta.session_state << "\n";

            const u_char* app_data = transport_data + sizeof(udp_header);
            int app_data_size = meta.payload_size - ipv6_hlen - sizeof(udp_header);
            if (app_data_size > 0) {
                meta.app_data = parseApplicationPayload(app_data, app_data_size, meta.protocol, meta.src_port, meta.dst_port, meta);
                meta.cipher_suite = meta.tls_version.empty() ? "None" : "Unknown (TLS detected)";
                meta.compression = "None";

                std::cout << "[Layer 6 - Presentation]\n";
                std::cout << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                std::cout << "Cipher Suite: " << meta.cipher_suite << "\n";
                std::cout << "Compression: " << meta.compression << "\n";
                logStream << "[Layer 6 - Presentation]\n";
                logStream << "TLS Version: " << (meta.tls_version.empty() ? "None" : meta.tls_version) << "\n";
                logStream << "Cipher Suite: " << meta.cipher_suite << "\n";
                logStream << "Compression: " << meta.compression << "\n";

                std::cout << "[Layer 7 - Application]\n";
                std::cout << "Protocol: " << meta.app_protocol << "\n";
                std::cout << "Application Data: " << meta.app_data << "\n";
                logStream << "[Layer 7 - Application]\n";
                logStream << "Protocol: " << meta.app_protocol << "\n";
                logStream << "Application Data: " << meta.app_data << "\n";
            }
        }

        // Update flow stats
        stats.packet_count++;
        stats.total_bytes += meta.payload_size;
        if (stats.last_timestamp) {
            double duration = timestamp - stats.last_timestamp;
            stats.byte_rate = duration > 0 ? meta.payload_size / duration : 0;
        }
    }
    else {
        meta.protocol = "Non-IP";
        meta.direction = "Unknown";
        meta.session_id = "N/A";
        meta.session_state = "N/A";
        meta.app_protocol = "Unknown";
        meta.app_data = "Non-IP packet";

        std::cout << "[Layer 3 - Network]\n";
        std::cout << "Non-IP packet, no IP addresses available\n";
        std::cout << "Direction: " << meta.direction << "\n";
        logStream << "[Layer 3 - Network]\n";
        logStream << "Non-IP packet, no IP addresses available\n";
        logStream << "Direction: " << meta.direction << "\n";

        std::cout << "[Layer 4 - Transport]\n";
        std::cout << "Protocol: None\n";
        logStream << "[Layer 4 - Transport]\n";
        logStream << "Protocol: None\n";

        std::cout << "[Layer 5 - Session]\n";
        std::cout << "Session ID: " << meta.session_id << "\n";
        std::cout << "Session State: " << meta.session_state << "\n";
        logStream << "[Layer 5 - Session]\n";
        logStream << "Session ID: " << meta.session_id << "\n";
        logStream << "Session State: " << meta.session_state << "\n";

        std::cout << "[Layer 6 - Presentation]\n";
        std::cout << "TLS Version: None\n";
        std::cout << "Cipher Suite: None\n";
        std::cout << "Compression: None\n";
        logStream << "[Layer 6 - Presentation]\n";
        logStream << "TLS Version: None\n";
        logStream << "Cipher Suite: None\n";
        logStream << "Compression: None\n";

        std::cout << "[Layer 7 - Application]\n";
        std::cout << "Protocol: " << meta.app_protocol << "\n";
        std::cout << "Application Data: " << meta.app_data << "\n";
        logStream << "[Layer 7 - Application]\n";
        logStream << "Protocol: " << meta.app_protocol << "\n";
        logStream << "Application Data: " << meta.app_data << "\n";
    }

    Logger::getInstance().logPacket(logStream.str());

    // Log flow stats every 100 packets
    if (packet_count % 100 == 0) {
        std::ostringstream flowStream;
        for (const auto& [pair, stats] : flow_stats) {
            flowStream << "Flow: " << pair.first << " -> " << pair.second << "\n";
            flowStream << "Packets: " << stats.packet_count << ", Bytes: "
                << stats.total_bytes << ", Rate: " << stats.byte_rate << " B/s\n";
        }
        Logger::getInstance().logFlowStats(flowStream.str());
    }

    std::cout << std::dec;
}