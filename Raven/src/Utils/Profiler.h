#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <iostream>

struct ProfileResult
{
	std::string m_name;
	long long m_start, m_end;
	uint32_t m_thread_id;
};

struct InstrumentationSession
{
	std::string m_name;
};

class Instrumentor
{
private:
	InstrumentationSession* m_curr_session;
	std::ofstream m_output;
	int m_profile_count;

public:

	Instrumentor() : m_curr_session(nullptr), m_profile_count(0) {}

	void begin_session(const std::string& name, const std::string& file_path = "results.json")
	{
		m_output.open(file_path);
		write_header();
		m_curr_session = new InstrumentationSession{ name };
	}

	void end_session()
	{
		write_footer();
		m_output.close();

		delete m_curr_session;
		m_curr_session = nullptr;
		m_profile_count = 0;
	}

	void write_profile(const ProfileResult& result)
	{
		if (m_profile_count++ > 0)
			m_output << ",";

		std::string name = result.m_name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_output << "{";
		m_output << "\"cat\":\"function\",";
		m_output << "\"dur\":" << (result.m_end - result.m_start) << ',';
		m_output << "\"name\":\"" << name << "\",";
		m_output << "\"ph\":\"X\",";
		m_output << "\"pid\":0,";
		m_output << "\"tid\":" << result.m_thread_id << ",";
		m_output << "\"ts\":" << result.m_start;
		m_output << "}";

		m_output.flush();
	}

	void write_header()
	{
		m_output << "{\"otherData\": {},\"traceEvents\":[";
		m_output.flush();
	}

	void write_footer()
	{
		m_output << "]}";
		m_output.flush();
	}

	static Instrumentor& get()
	{
		static Instrumentor m_instance;
		return m_instance;
	}
};

class Timer
{
public:
	Timer(const char* name)
		: m_name(name), m_stopped(false)
	{
		m_start_timepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_stopped)
			stop();
	}

	void stop()
	{
		auto end_timepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_timepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();

		uint32_t thread_id = uint32_t(std::hash<std::thread::id>{}(std::this_thread::get_id()));
		Instrumentor::get().write_profile({ m_name, start, end, thread_id });

		m_stopped = true;

		// For terminal printing
		std::cerr << "\n";
		std::cerr << m_name << " " << (end - start) / 1000 <<" ms" << std::endl;
		std::cerr << "\n";

	}
private:
	const char* m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_timepoint;
	bool m_stopped;
};


//#define PROFILING 0
#if RAVEN_PROFILING

#define RAVEN_PROFILE_BEGIN_SESSION(name, filepath) Instrumentor::get().begin_session(name, filepath)
#define RAVEN_PROFILE_END_SESSION() Instrumentor::get().end_session()
#define RAVEN_PROFILE_SCOPE(name) Timer timer##__LINE__(name);
#define RAVEN_PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
#define RAVEN_PROFILE_BEGIN_SESSION(name, filepath) 
#define RAVEN_PROFILE_END_SESSION()
#define RAVEN_PROFILE_SCOPE(name) 
#define RAVEN_PROFILE_FUNCTION() 
#endif
