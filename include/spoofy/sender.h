#ifndef _SENDER_H_
#define _SENDER_H_

#include <memory>
#include <string_view>

#include <tins/tins.h>
// maybe #include <librdkafka/rdkafkacpp.h>
#include <rdkafkacpp.h>

class SendingStrategy {
public:
    virtual ~SendingStrategy() {}  // implement this in cpp file or all hell breaks loose
    virtual void send(Tins::PDU &p) = 0;
};

/** 
 * @brief Context for sending and receiving packets with different strategies
 */
class Sender {
public:
    Sender(std::unique_ptr<SendingStrategy> sender = nullptr);
    ~Sender();

    void send_packet(Tins::Packet &p);

    void set_sender(std::unique_ptr<SendingStrategy> send_strategy);
private:
    std::unique_ptr<SendingStrategy> sender_;
};

 /** 
  * @brief Sends packets using libtins, on specified interface
  * @code
  *     auto pkt = Tins::EthernetII(eth.src_addr(), eth.dst_addr()) /
  *         Tins::IP(ip.src_addr(), ip.dst_addr()) /
  *         Tins::UDP(udp.sport(), udp.dport());
  *     SendingContext sc(std::make_unique<NetworkSender>("eth0");
  *     sc.send(pkt)
  * @endcode
 */
class NetworkSender : public SendingStrategy {
public:
    NetworkSender(const char *interface);
    void set_interface(const char *interface);
private:
    virtual void send(Tins::PDU &pdu);

    Tins::NetworkInterface interface_;
    Tins::PacketSender packet_sender_;
};

 /** 
  * @brief Sends packet to Apache Kafka using librdkafka
  * @code
  *     auto pkt = Tins::EthernetII(eth.src_addr(), eth.dst_addr()) /
  *         Tins::IP(ip.src_addr(), ip.dst_addr()) /
  *         Tins::UDP(udp.sport(), udp.dport());
  *     SendingContext sc(std::make_unique<KafkaSender>(params);
  *     sc.send(pkt)
  * @endcode
 */
class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
    void dr_cb(RdKafka::Message &message);
};
class KafkaSender : public SendingStrategy {
public:
    // TODO This can take another parameter in the future, representing a output data format we want to send to Kafka,
    // KafkaSender(const char *brokers, const char *topic, KafkaFormat fmt);
    // for now we will keep the only output format as JSON, hardcoded in this class, but we might want others as well
    KafkaSender(const char *brokers, const char *topic);
    ~KafkaSender();
private:
    virtual void send(Tins::PDU &pdu);
    std::string jsonify(Tins::PDU &pdu);

    ExampleDeliveryReportCb ex_dr_cb_;
    RdKafka::Producer *producer_;
    std::string brokers_, topic_;
};

#endif // _SENDER_H_
