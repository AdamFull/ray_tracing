#pragma once

#include <atomic>
#include <queue>
#include <unordered_map>

template<class _Ty>
class resource_container
{
public:
	~resource_container()
	{
		perform_delete();
	}

	size_t add(const std::string& name, std::unique_ptr<_Ty>&& object)
	{
		auto id = get_id(name);
		if (id != invalid_index)
		{
			usage_counter_map[id].fetch_add(1u, std::memory_order_relaxed);
			return id;
		}

		name_to_id_map.emplace(name, next_id);
		id_to_resource_map.emplace(next_id, std::move(object));
		usage_counter_map[next_id].fetch_add(1u, std::memory_order_relaxed);
		perform_next_id();
		return name_to_id_map[name];
	}

	void increment(const std::string& name)
	{
		increment(get_id(name));
	}

	void increment(size_t id)
	{
		if (id != invalid_index)
			usage_counter_map[id].fetch_add(1u, std::memory_order_relaxed);
	}

	void remove(const std::string& name)
	{
		auto nameit = name_to_id_map.find(name);
		if (nameit != name_to_id_map.end())
			remove(nameit->second);
	}

	void remove(size_t id)
	{
		auto objit = id_to_resource_map.find(id);
		if (objit != id_to_resource_map.end())
		{
			auto usages = usage_counter_map.find(id);
			usages->second.fetch_sub(1u, std::memory_order_relaxed);
			if (usages->second == 0ull)
			{
				free_ids.push(id);
				delete_queue.push(std::move(objit->second));
				id_to_resource_map.erase(objit);
				usage_counter_map.erase(usages);

				auto nameit = std::find_if(name_to_id_map.begin(), name_to_id_map.end(), [id](const auto& kv) { return kv.second == id; });
				if (nameit != name_to_id_map.end())
					name_to_id_map.erase(nameit);
			}
		}
	}

	size_t get_id(const std::string& name)
	{
		auto nameit = name_to_id_map.find(name);
		if (nameit != name_to_id_map.end())
			return nameit->second;
		return invalid_index;
	}

	const std::unique_ptr<_Ty>& get(const std::string& name)
	{
		static std::unique_ptr<_Ty> empty{ nullptr };

		auto id = get_id(name);
		if (id != invalid_index)
			return get(id);

		return empty;
	}

	const std::unique_ptr<_Ty>& get(size_t id)
	{
		static std::unique_ptr<_Ty> empty{ nullptr };

		auto objit = id_to_resource_map.find(id);
		if (objit != id_to_resource_map.end())
			return objit->second;

		return empty;
	}

	void perform_delete()
	{
		while (!delete_queue.empty())
			delete_queue.pop();
	}

	const std::unordered_map<resource_id_t, std::unique_ptr<_Ty>>& get_all() const 
	{ 
		return id_to_resource_map;
	}

private:
	void perform_next_id()
	{
		static std::atomic<resource_id_t> uid{ 0 };

		if (!free_ids.empty())
		{
			next_id = free_ids.front();
			free_ids.pop();
		}
		else
		{
			uid.fetch_add(1u, std::memory_order_relaxed);

			if (uid == invalid_index)
				uid.fetch_add(1u, std::memory_order_relaxed);

			next_id = uid;
		}
	}

private:
	resource_id_t next_id{ 0 };
	std::queue<std::unique_ptr<_Ty>> delete_queue;
	std::queue<resource_id_t> free_ids;
	std::unordered_map<resource_id_t, std::atomic<resource_id_t>> usage_counter_map;
	std::unordered_map<std::string, resource_id_t> name_to_id_map;
	std::unordered_map<resource_id_t, std::unique_ptr<_Ty>> id_to_resource_map;
};