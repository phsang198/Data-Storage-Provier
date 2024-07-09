#pragma once
#include "pch.h"
#include <string> 
#include <iostream> 
#include <fstream>
#include "S3W/S3Wrapper.h"
#include <experimental/filesystem>

using namespace std; 

class CProvider
{
public: 
	CProvider(const std::string& _name, const std::string& _type, const std::string& _accessKeyID, const std::string& _secretAccessKey)
	{
		name = _name;
		type = _type;
		sc.accessKeyID = _accessKeyID;
		sc.secretAccessKey = _secretAccessKey;
	}
	~CProvider() {};
public:
	struct secret_config
	{
		std::string accessKeyID;
		std::string secretAccessKey;
	};
public:
	std::string name; 
	std::string type; 
	secret_config sc; 
	
public:
	virtual std::string getObject(const std::string& _key) = 0; 
	virtual void getListNameObject(const std::string& _key, std::string subKey, std::vector<std::string>& listObjectKey) = 0; 
	virtual void getListObject(const std::string& _key, const std::string& _path, std::string subKey) = 0;
	virtual BOOL initObject(const std::string& _key, const std::string& _path) = 0; 
	virtual BOOL updateObject(const std::string& _key, const std::string& _data) = 0;
	virtual BOOL deleteObject(const std::string& _path) = 0;
	virtual BOOL deleteListObject(const std::string& _path, const std::string& subKey) = 0;
};

class s3w : public CProvider 
{
public:
	s3w(const std::string& _name, const std::string& _type, const std::string& _accessKeyID, const std::string& _secretAccessKey,
		const std::string& _host, const std::string& _bucketName, const std::string& _region) : CProvider(_name, _type, _accessKeyID, _secretAccessKey)
	{
		param.host = _host;
		param.bucketName = _bucketName;
		param.region = _region;
		Aws::InitAPI(_awsOptions);
		_putenv("AWS_EC2_METADATA_DISABLED=true");
		awsclient = CS3Wrapper::NewS3Client(_host, sc.accessKeyID, sc.secretAccessKey, _region);	
	}
	~s3w() { Aws::ShutdownAPI(_awsOptions); };
private:
	
	Aws::SDKOptions		_awsOptions;
public:

	struct parameters
	{
		std::string host;
		std::string bucketName;
		std::string region;
	};
	parameters param; 
	std::shared_ptr<Aws::S3::S3Client> awsclient; 

public:
	void getListNameObject(const std::string& _key, std::string subKey, std::vector<std::string>& listObjectKey)
	{
		std::vector<std::string> list;

		bool tmp = CS3Wrapper::ListObjects(awsclient, param.bucketName, _key, list);
		if (tmp)
		{
			for (auto x : list)
			{
				int pos = StringProcess::existSubStr(x, subKey);

				if (pos != -1)
				{
					std::string file_name = StringProcess::getNameFile(x, '/');
					listObjectKey.push_back(file_name);
				}
			}
		}
	}
	void getListObject(const std::string& _key, const std::string& _path,std::string subKey)
	{
		std::vector<std::string> listObjectKey; 
		bool tmp = CS3Wrapper::ListObjects(awsclient, param.bucketName, _key, listObjectKey);
		if (tmp)
		{
			for (auto x : listObjectKey)
			{
				int pos = StringProcess::existSubStr(x, subKey);
				if (pos != -1)
				{
					std::string res = "";
					std::string file_name = StringProcess::getNameFile(x, '/');
					std::string extens = StringProcess::getExtensionFile(file_name);
					bool tmp1 = CS3Wrapper::GetObject(awsclient, param.bucketName, x, res);
					if (res != "")
					{
						std::string path = _path + '/' + file_name;

						if (extens == "geojson" || extens == "osm")
						{
							std::ofstream fo(path, std::ios::out);
							fo << res;
							fo.close();
						}
						else
						{
							std::ofstream fo(path, std::ios::binary);
							fo << res;
							fo.close();
						}
					}
				}
			}
		}
		return ;
	}

	std::string getObject(const std::string& _key)
	{
		std::string res = ""; 
		bool tmp = CS3Wrapper::GetObject(awsclient, param.bucketName , _key , res );
		return res ; 
	}
	BOOL initObject(const std::string& _key, const std::string& _path)
	{
		bool test = CS3Wrapper::PutObjectFile(awsclient, param.bucketName, _key, _path);
		return test;
	}
	BOOL updateObject(const std::string& _key, const std::string& _data)
	{
		bool test = CS3Wrapper::PutObjectData(awsclient, param.bucketName, _key , _data);
		return test; 
	}
	BOOL deleteListObject(const std::string& _path, const std::string& subKey)
	{
		std::vector<std::string> listObjectKey;
		bool tmp = CS3Wrapper::ListObjects(awsclient, param.bucketName, _path, listObjectKey);
		bool test = true ; 
		if (tmp)
		{
			for (auto x : listObjectKey)
			{
				int pos = StringProcess::existSubStr(x, subKey);
				if (pos != -1)
				{
					 test = CS3Wrapper::DeleteObject(awsclient, param.bucketName, x) && test ;
				}
			}
		}
		return test;
	}
	BOOL deleteObject(const std::string& _path)
	{
		bool test = CS3Wrapper::DeleteObject(awsclient, param.bucketName, _path);
		return test;
	}

};

class directory : public CProvider
{
public:
	directory(const std::string& _name, const std::string& _type, const std::string& _accessKeyID, const std::string& _secretAccessKey, 
			  const std::string& _host, const std::string& _folder) : CProvider( _name, _type, _accessKeyID, _secretAccessKey)
	{
		param.host = _host;
		param.folder = _folder;
	}
	~directory()
	{

	}
public:

	struct parameters
	{
		std::string host;
		std::string folder;
	};
	parameters param;
private: 
	bool ListObjects(const std::string& _key, std::vector<std::string>& listObjectKey)
	{
		std::string path = param.host + param.folder + _key ;
		//path = "\\\\329-L1-PC06\\ntlong" ; 
		for (auto& p : std::experimental::filesystem::directory_iterator(path)) {
			listObjectKey.push_back(StringProcess::ws2string(p.path())); 
		}
		if (listObjectKey.empty())return false; 
		return  true; 
	}
	std::string getData(const std::string& path)
	{
		std::ifstream input(path, std::ios::binary);
		std::ostringstream ostrm;
		ostrm << input.rdbuf();
		return std::string(ostrm.str());
	}
	BOOL deleteFile(const std::string& path)
	{
		int ret = remove(path.c_str());
		bool is_ok = (ret == 0) ? true : false;
		return is_ok;
	}
	BOOL deleteFolder(const std::string& path)
	{
		Poco::File aFile(path);
		if (aFile.exists() && aFile.isDirectory())						// xoa folder 
		{
			aFile.remove(true);
			return TRUE;
		}
		return false;

	}
public:
	void getListNameObject(const std::string& _key, std::string subKey, std::vector<std::string>& listObjectKey)
	{
		std::vector<std::string> list;

		bool tmp = directory::ListObjects(_key, list);
		if (tmp)
		{
			for (auto x : list)
			{
				int pos = StringProcess::existSubStr(x, subKey);

				if (pos != -1)
				{
					std::string file_name = StringProcess::getNameFile(x, '\\');
					listObjectKey.push_back(file_name);
				}
			}
		}
	}
	void getListObject(const std::string& _key, const std::string& _path, std::string subKey)
	{
		std::vector<std::string> listObjectKey;
		bool tmp = directory::ListObjects(_key, listObjectKey);
		if (tmp)
		{
			for (auto x : listObjectKey)
			{
				int pos = StringProcess::existSubStr(x, subKey);
				if (pos != -1)
				{
					std::string file_name = StringProcess::getNameFile(x, '\\');
					std::string extens = StringProcess::getExtensionFile(file_name);
					std::string sub_path = x; 

					std::string res = directory::getData(sub_path);
					if (res != "")
					{
						std::string path = _path + '/' + file_name;

						if (extens == "geojson" || extens == "osm")
						{
							std::ofstream fo(path, std::ios::out);
							fo << res;
							fo.close();
						}
						else
						{
							std::ofstream fo(path, std::ios::binary);
							fo << res;
							fo.close();
						}
					}
				}
			}
		}
		return;
	}
	std::string getObject(const std::string& _key)
	{
		std::string path = param.host + param.folder + _key;
		std::ifstream input(path, std::ios::binary);
		std::ostringstream ostrm;
		ostrm << input.rdbuf();
		input.close(); 
		return std::string(ostrm.str());
	}
	BOOL initObject(const std::string& _key, const std::string& _path)
	{
		std::string des_path = param.host + param.folder + _key;
		std::string source_path = _path;
		std::ifstream fi(source_path , std::ios::binary);
		std::ofstream fo(des_path, std::ios::binary); 

		std::ostringstream ostrm;
		ostrm << fi.rdbuf();
		fo << ostrm.str(); 

		fi.close();
		fo.close();
		return TRUE;
	}
	BOOL updateObject(const std::string& _key, const std::string& _data)
	{
		std::string path = param.host + param.folder + _key;

		std::string folder = StringProcess::getPathExceptNameFile(path); 

		Poco::File aFile(folder);
		if (!aFile.exists())
		{
			aFile.createDirectories();
		}

		std::ofstream fo(path, std::ios::binary);
		fo << _data; 
		fo.close(); 
		return TRUE; 
	}
	BOOL deleteListObject(const std::string& _path, const std::string& subKey)
	{
		std::vector<std::string> listObjectKey;
		bool tmp = directory::ListObjects(_path, listObjectKey);
		bool test = true;
		if (tmp)
		{
			for (auto x : listObjectKey)
			{
				int pos = StringProcess::existSubStr(x, subKey);
				if (pos != -1)
				{
					Poco::File aFile(x);
					if (aFile.exists() && aFile.isFile())
					{
						test = directory::deleteFile(x) && test;							// xoa file 
					}
					else
					{
						test = directory::deleteFolder(x) && test;
					}
				}
			}
			std::string path = param.host + param.folder + _path ;
			Poco::File aFile(path);

			//int k = aFile.totalSpace(); 
			if (aFile.exists() && aFile.isDirectory() && aFile.totalSpace() <= 0)
				test = directory::deleteFolder(path) && test;
		}
		return test;
	}
	BOOL deleteObject(const std::string& _path)
	{
		std::string path = param.host + param.folder + _path;
		int ret = remove(path.c_str());
		bool is_ok = (ret == 0) ? true : false;
		return is_ok;
	}
};