// Подключим библиотеку Boost.Optional, чтобы убедиться, что Boost подключен успешно
#include <google/protobuf/util/json_util.h>
#include <iostream>
#include <fstream>

using namespace std::string_literals;
/*
enum class PayloadType : unsigned int
{
	CLIENT_TYPE,
	SERVER_TYPE
};

std::ostream &operator<<(std::ostream &out, const Serialize::Server &server) {
  out << server.server() << " ("s << server.server_id() << "): "s << server.location();
  return out;
}
 
template<typename P>
inline std::ostream &Print(
    const Serialize::Envelope &envelope,
    std::ostream &out = std::cout
) {
  out << "From: " << envelope.sender() << std::endl;
  out << "Subject: " << envelope.message() << std::endl;
 
  if (envelope.has_payload()) {
    out << "Type: " << envelope.payload().type_url()
        << " (" << envelope.payload_type_id() << ")" << std::endl;
    out << "---- Payload ----" << std::endl;
    const auto &payload = envelope.payload();
    //https://protobuf.dev/reference/cpp/cpp-generated/#any https://codetrips.com/2021/01/04/implement-the-envelope-wrapper-pattern-in-c-using-google-protocol-buffers/ https://firebase.google.com/docs/storage/cpp/start?hl=ru
    if (payload.Is<P>()) {
      P content;
      payload.UnpackTo(&content);
      std::cout << content << std::endl;
    }
    out << "---- Payload Ends ----" << std::endl;
  }
  return out;
}

int main() {

    Serialize::Envelope envelope;
    envelope.set_sender("demo-prog");
    envelope.set_message("This is a demo");
 
    Serialize::Server server;
    server.set_server("host.kapsules.io");
    server.set_server_id(9999);
    server.set_location("us-west-2a");
    
    // Use the Any::PackFrom to encode an arbitrary PB payload.
    envelope.mutable_payload()->PackFrom(server);
    envelope.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_TYPE));
    
    if (static_cast<::google::protobuf::uint32>(PayloadType::SERVER_TYPE) == envelope.payload_type_id()) 
     {
	    Print<Serialize::Server>(envelope);
     } 
     else 
     {
	    std::cout << "Unknown payload type (" << envelope.payload_type_id() << ")";
     }
    
    Serialize::Address addr;
    addr.set_building(15);

    std::cout << "building:" << addr.building() << std::endl;
    
    Serialize::Person p;
    p.set_name("Ivan");
    p.add_phone_number("+9-888-777-66-55");
    p.add_phone_number("+9-888-877-78-56");

  for(const std::string& phone: p.phone_number())
  std::cout << phone << std::endl;
  
  std::ofstream out("person.bin", std::ios_base::binary);
  p.SerializePartialToOstream(&out);
  out.close();
    
  std::ifstream inp("person.bin", std::ios_base::binary);
  Serialize::Person parsed_person;
  if(parsed_person.ParseFromIstream(&inp))
  {
  std::cout << "Name:" << parsed_person.name() << std::endl;
  }
  
  std::string json_string;
  Serialize::SearchRequest sr, sr2;

  // Populate sr.
  sr.set_query(std::string("Hello!"));
  sr.set_page_number(1);
  sr.set_result_per_page(10);

  // Create a json_string from sr.
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  options.preserve_proto_field_names = true;
  MessageToJsonString(sr, &json_string, options);

  // Print json_string.
  std::cout << json_string << std::endl;
  
  google::protobuf::util::JsonParseOptions options2;
  JsonStringToMessage(json_string, &sr2, options2);

  // Print the values of sr2.
  std::cout
    << sr2.query() << ", "
    << sr2.page_number() << ", "
    << sr2.result_per_page() << std::endl
  ;
  
    return 0;
}	
*/

