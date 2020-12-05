#ifndef RIGI_SETTING_H_
#define RIGI_SETTING_H_

namespace Rigitaeda
{
	enum class _OUTPUT_TYPE
	{
		LOG,
		FILE,
		TCP,
	};

	class Rigi_Filter
	{
	public:
		Rigi_Filter() {};
		virtual ~Rigi_Filter() {};

	};

	class Rigi_Output
	{
	public:
		Rigi_Output() {};
		virtual ~Rigi_Output() {};

	private:
		_OUTPUT_TYPE m_eType = _OUTPUT_TYPE::FILE;

	public:
		_OUTPUT_TYPE GetType()
		{
			return m_eType;
		}
	};

	class Rigi_Output_File
	{
	public:
		Rigi_Output_File() {};
		virtual ~Rigi_Output_File() {};

	private:
		_OUTPUT_TYPE m_eType = _OUTPUT_TYPE::FILE;

	public:
		_OUTPUT_TYPE GetType()
		{
			return m_eType;
		}
	};

	class Rigi_Output_TCP
	{
	public:
		Rigi_Output_TCP() {};
		virtual ~Rigi_Output_TCP() {};

	private:
		_OUTPUT_TYPE m_eType = _OUTPUT_TYPE::FILE;

	public:
		_OUTPUT_TYPE GetType()
		{
			return m_eType;
		}
	};
};

#endif