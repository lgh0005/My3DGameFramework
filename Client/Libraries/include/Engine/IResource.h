#pragma once

class IResource : public enable_shared_from_this<IResource>
{
public:
	IResource(const string& name, Resources::ResourceType type);
	virtual ~IResource() = default;

public:
	Resources::ResourceType GetType() { return _type; }
	const string& GetName() { return _name; }

protected:
	virtual void Load(const string& path) {}
	virtual void Save(const string& path) {}

protected:
	Resources::ResourceType _type = Resources::ResourceType::Unknown;
	string _name;
	string _path;
};

