#ifndef OUTPUT_H
#define OUTPUT_H

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "RegisterSubclass.h"

namespace Output {

class Base {
       public:
	class Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) = 0;
		virtual ~Factory() {}
	};

       private:
	static auto& factories() {
		static std::map<std::string, std::unique_ptr<Factory>> f;
		return f;
	};
	template <typename Base, typename Child>
	friend RegisterSubclass<Base, Child>::Register::Register();

       public:
	static const auto& get_factories() { return factories(); }
	virtual void write_header(const std::vector<std::string>&) = 0;
	virtual void write_record(const std::vector<double>&) = 0;
	virtual ~Base() = default;
};

class CSVFile : public Base {
       private:
	std::unique_ptr<std::ostream> out;
	bool header;

       public:
	static constexpr char type_name[] = "CSVFile";
	class Factory : public Base::Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) override;
	};
	CSVFile() = delete;
	CSVFile(const std::string& path, bool header);
	void write_header(const std::vector<std::string>&) override;
	void write_record(const std::vector<double>&) override;
};

}  // namespace Output
template class RegisterSubclass<Output::Base, Output::CSVFile>;

namespace YAML {

template <>
std::unique_ptr<Output::Base> Node::as() const;

}  // namespace YAML

#endif  // OUTPUT_H
