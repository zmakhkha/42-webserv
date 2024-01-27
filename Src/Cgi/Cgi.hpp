#pragma once

#include "../ConfigFile/myconfig.hpp"
// #include "../Response/Response.hpp"

#define BAD_GATEWAY 502
#define st_ std::string
class request;

class Cgi
{
private:
	st_ SERVER_SOFTWARE;
	st_ SERVER_NAME;
	st_ GATEWAY_INTERFACE;

	st_ _uri, _methode;
	int _location;
	st_ upload_path;

	std::vector<std::string> _envLst;
	std::map<std::string, std::string> _reqHeaders;

	std::string _CgiScriptPath;
	std::string _scriptPath;

	Config configuration;
	bool _isPost;

	std::string _postBody;
	std::string _respPath;
	Server srv;
	bool forked;
	pid_t pid;
	bool status;

public:
	Cgi();
	bool cgiDone;
	Cgi(st_ uri, st_ methode, int loc, st_ cgiRes, std::map<st_, st_> heads, st_ upPath, Server _srv);
	Cgi(int var);
	Cgi(const Cgi &tmp);
	Cgi &operator=(const Cgi &tmp);
	~Cgi();
	void setHeaders(const std::map<std::string, const std::string> headers);
	void formatHeaders();
	void setEnv();
	void headToEnv(std::string &str);
	void execute();
	void formatKey(std::string &key);
	void excecCgi(std::string bodyPath);
	void setExtraEnv();
	// st_ getRespPath(void) const;
	void setUnique();

	std::string getQueryStr();
	std::pair<st_, st_> getPathQuery(st_ uri);
	void setUri(const st_ &uri);
	st_ getUri() const;
	void setMethode(const st_ &methode);
	st_ getMethode() const;
	void setLocation(int location);
	int getLocation() const;
	void setUploadPath(const st_ &uploadPath);
	st_ getUploadPath() const;
	void setReqHeaders(const std::map<st_, st_> &reqHeaders);
	std::map<st_, st_> getReqHeaders() const;
	void setRespPath(const st_ &respPath);
	st_ getRespPath() const;
	void setServer(const Server &server);
	Server getServer() const;
};