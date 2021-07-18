#include "FileSearcher.h"
#include <iostream>
#include <algorithm>

FileSearcher::FileSearcher() : typeOfSearch(SearchType::NotSet)
{
	m_thredID = std::this_thread::get_id();
	max_threads.store(std::thread::hardware_concurrency()); //This number is just sugestion - normally test to find optimal number
}

void FileSearcher::searchFile()
{
	using namespace std::string_literals;

	std::unique_lock lk(m_mutex);
	while (!pendingRoots.empty())
	{
		try
		{
			char c = *pendingRoots.begin();
			fs::path rootPath(char(std::toupper(c)) + ":\\"s);

			if (pendingRoots.size() > 1 && m_threads.load() < max_threads) 
			{
				std::future future = std::async(std::launch::async,
					&FileSearcher::searchOnDisk, this, rootPath);
				futures.push_back(std::move(future));
				++m_threads;
				pendingRoots.erase(pendingRoots.begin());
			}
			else 
			{
					pendingRoots.erase(pendingRoots.begin()); 
					lk.unlock();
					searchOnDisk(rootPath);
					lk.lock(); 
			}
		}
		catch (fs::filesystem_error& error)
		{
			std::wcerr << "Error occured: " << error.what() << '\n'
				<< "at path: " << error.path1() << ".\n\n";
		}
	}
	lk.unlock();
	retrieveFutures();
}

void FileSearcher::searchOnDisk(fs::path rootPath)
{
	//Search exclude sym_links
	for (const auto& i : fs::recursive_directory_iterator(rootPath, fs::directory_options::skip_permission_denied))
	{
		std::wstring entryName = getEntryName(i.path());
		if (!entryName.empty() && entryName == currFile)
		{
			std::lock_guard lk_guard(m_mutex);
			findResults.insert({ fs::absolute(i.path()), i.status().type() });
		}

		if (!pendingRoots.empty() && m_threads < max_threads )
		{
			std::lock_guard lk_guard(m_mutex); // Closed so you won't start bunch of tasks at the same time
			distribute_work();
		}
	}

	if(std::this_thread::get_id() != m_thredID)
		--m_threads;
}

/*Start a new thread in order to search another disk*/
void FileSearcher::distribute_work()
{
	using namespace std::string_literals;
	while (!pendingRoots.empty())
	{
		try
		{
			if (m_threads.load() < max_threads)
			{
				char c = *pendingRoots.begin();
				fs::path rootPath(char(std::toupper(c)) + ":\\"s);

				std::future future = std::async(std::launch::async,
					&FileSearcher::searchOnDisk, this, rootPath);
				futures.push_back(std::move(future));

				++m_threads;
				pendingRoots.erase(pendingRoots.begin());
			}
			else
				return;
		}

		catch (fs::filesystem_error& error)
		{
			std::wcerr << "Error occured: " << error.what() << '\n'
				<< "at path: " << error.path1() << ".\n\n";
		}
	}
}

std::wstring FileSearcher::getEntryName(const fs::path& path) const 
{
	std::wstring entryName;

	switch (typeOfSearch)
	{
	case SearchType::Extension:
		entryName = path.extension().wstring();
		break;

	case SearchType::FullName:
		entryName = path.filename().wstring();
		break;

	case SearchType::Stem:
		entryName = path.stem().wstring();
		break;
	}
	return entryName;
}

void FileSearcher::displaySearchResults(chr::milliseconds time)
{
	std::cout << "Searching for the file took: " << time.count() << "ms.\n";

	if (!findResults.empty())
		std::cout << "The subsequent file(s) has been found:\n";
	else
	{
		std::cout << "File has not been found.\n\n";
		return;
	}
	

	for (const auto& [path, type] : findResults) 
	{
		getEntryType(type);

		std::wcout << getEntryType(type) << " at path: "
			<< path.lexically_normal().wstring() << '\n';
		
	}
	std::cout << '\n';
}

void FileSearcher::retrieveFutures()
{
	for (auto& i : futures)
	{
		i.get();
	}
	futures.clear();
}

std::wstring FileSearcher::getEntryType(fs::file_type type) const 
{
	using namespace std::string_literals;

	switch (type)
	{
	case std::filesystem::file_type::unknown:
		[[fallthrough]];
	case std::filesystem::file_type::none:
		return L"Unindetified file";

	case std::filesystem::file_type::regular:
		return L"Regular file";

	case std::filesystem::file_type::directory:
		return L"Directory";

	case std::filesystem::file_type::block:
		return L"Block file";

	case std::filesystem::file_type::character:
		return L"Character file";

	case std::filesystem::file_type::fifo:
		return L"FIFO file";

	case std::filesystem::file_type::socket:
		return L"Socket file";

	case std::filesystem::file_type::junction:
		return L"OS specific file";

	default:
		return L"";
	}
}

void FileSearcher::setFileName(const std::wstring& name)
{
	currFile = name;
	setTypeOfSearch();
}

void FileSearcher::setTypeOfSearch()
{
	if (currFile[0] == '.')
		typeOfSearch = SearchType::Extension;

	else if (currFile.find('.') != std::string::npos)
		typeOfSearch = SearchType::FullName;

	else
		typeOfSearch = SearchType::Stem;
}

void FileSearcher::showAdvice()
{
	std::cout << "If you want to search another file press 1.\n"
		<< "If you want to stop searching press 0.\n"
		<< "If you want to hide helping messages press 5.\n"
		<< "If you want to show helping messages press 6.\n"
		<< "If you want to change root directories press 9.\n\n";
}

void FileSearcher::setFlags()
{
	char option = '5';
	do
	{
		std::cout << "Enter option: ";
		std::cin >> option;

		if (std::cin.rdstate() & std::ios_base::failbit)
		{
			std::cin.clear();
			continue;
		}

		switch (option)
		{
		case '0':
			cntFlag = false;
			std::cout << "Search of files is stopped.\n\n";
			break;

		case '1':
			pendingRoots.resize(rootNames.size());
			std::copy(rootNames.begin(), rootNames.end(), pendingRoots.begin());

			cntFlag = true;
			std::cout << "Search of files is continued.\n\n";
			break;

		case '5':
			adviceFlag = false;
			std::cout << "Helping messages are now hidden.\n\n";
			break;

		case '6':
			adviceFlag = true;
			std::cout << "Helping messages are now shown.\n\n";
			break;

		case '9':
			rootNames.clear();

			std::cout << "You are changing root names on which searching will take place (previous root names are discarded)\n"
				"inserting non-letter value will stop collecting root names: ";
			setRootNames();

			std::cout << '\n';
			break;

		default:
			std::cout << "Invalid character\n\n";
		}

	} while (option != '0' && option != '1');

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear stream before reading from getline
}

/*This function only check if there are multiple occurences of root names - it doesn't check
whether root name is correctly formed under OS indications*/
void FileSearcher::checkRootNames()
{
	using namespace std::string_literals;

	auto it = std::unique(rootNames.begin(), rootNames.end());
	rootNames.erase(it, rootNames.end());

	int i = 0;
	for (auto& i : rootNames)
	{
		fs::path rootPath(char(std::toupper(i)) + ":\\"s);

		if (!fs::exists(rootPath)) // remove incorrent roots
		{
			rootNames.erase(rootNames.begin() + i);
			pendingRoots.erase(rootNames.begin() + i);
			continue;
		}
		++i;
	}
}

/*This functions assume that you are using Windows. The root names may be incorrect for other platforms.*/
void FileSearcher::setRootNames()
{
	char volume;

	while (std::cin >> volume)
	{
		if (std::isalpha(volume))
			rootNames.push_back(volume);
		else
			break;
	}
	checkRootNames();

	if (std::cin.rdstate() & std::ios_base::failbit)
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	pendingRoots.resize(rootNames.size());
	std::copy(rootNames.begin(), rootNames.end(), pendingRoots.begin());
}
