#include "file.h"

namespace hbqj {
	Position File::ReadPosition() {
		std::string json_str = R"({"x":1.100000023841858,"y":2.200000047683716,"z":3.299999952316284,"w":123})";

		json j = json::parse(json_str);
		log.info("{}", j.dump());

		auto position = j.get<Position>();
		log.info("{}", position);

		return position;
	}

	void File::SavePosition() {
		Position position{ .x = 1.1, .y = 2.2, .z = 3.3 };
		json j = position;

		log.info("{}", j.dump());

		std::ofstream json_file("data.json", std::ios::out);
		json_file << j;
		json_file.close();
		log.info("write to data.json");

		std::ifstream in_json_file("data.json", std::ios::in);
		json in_j;
		in_json_file >> in_j;
		in_json_file.close();

		log.info("json from file: {}", in_j.dump());

		std::filesystem::remove("data.json");
		//		try {
		//			Position pos = in_j.template get<Position>();
		//		}
		//		catch (const json::exception& e)
		//		{
		//			std::cout << "deserialization failed: " << e.what() << std::endl;
		//		}
	}
}