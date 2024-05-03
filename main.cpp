#include <iostream>
#include <string>
#include <shlwapi.h> // For ShellExecuteA
#include "HttpClient.h" // Assuming HttpClient.h contains the HttpClient class
#include <conio.h> // For _getch
#include "json.hpp" // For JSON parsing
#include <iomanip> // For setw
#include <fstream>

#pragma warning( push )
#pragma warning(disable:26819) // Disabling warning 26819
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;

// Define a class representing a Movie
class Movie {
public:
    int id;
    string overview;
    double popularity;
    string poster_path;
    string release_date;
    string title;
    double vote_average;
    int vote_count;

    // Overloading << operator for easy printing
    friend ostream& operator<<(ostream& os, const Movie& movie) {
        os << "ID: " << movie.id << ", Title: " << movie.title;
        return os;
    }
};

// Comparator struct to compare movies by title
struct CompareTitle {
    bool operator()(const Movie* m1, const Movie* m2) const {
        return m1->title < m2->title;
    }
};

// Comparator struct to compare movies by popularity
struct ComparePopularity {
    bool operator()(const Movie* m1, const Movie* m2) const {
        return m1->popularity < m2->popularity;
    }
};

// Define a template class for a dynamic array
template<typename T>
class DynamicArray {
private:
    T* arr;
    int size;
public:
    DynamicArray(int initialSize = 20) : size(initialSize) {
        arr = new T[size];
    }

    ~DynamicArray() {
        delete[] arr;
    }

    void Resize(int newSize) {
        T* newArr = new T[newSize];
        for (int i = 0; i < min(size, newSize); ++i) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = newArr;
        size = newSize;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw out_of_range("Index out of bounds");
        }
        return arr[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw out_of_range("Index out of bounds");
        }
        return arr[index];
    }
};

// Class representing a client to interact with TMDB API
class MovieClient : public HttpClient {
private:
    DynamicArray<Movie*> movies; // Dynamic array to store movies
    size_t movieCount = 0; // Number of movies

    // Method to delete memory allocated to movies
    void DeleteMemory() {
        for (int i = 0; i < movieCount; ++i) {
            delete movies[i];
        }
    }

public:
    MovieClient() {}

    ~MovieClient() {
        DeleteMemory();
    }

    // Template method to perform bubble sort on movies
    template<typename T>
    void BubbleSort(bool ascending) {
        // we used vector for sorting data
        vector<Movie*> tempMovies;
        for (int i = 0; i < movieCount; ++i) {
            tempMovies.push_back(movies[i]);
        }

        T comparator;
        for (int i = 0; i < movieCount - 1; ++i) {
            for (int j = 0; j < movieCount - i - 1; ++j) {
                if (ascending ? comparator(movies[j + 1], movies[j]) : comparator(movies[j], movies[j + 1])) {
                    swap(movies[j], movies[j + 1]);
                }
            }
        }
    }

    // Method to get a movie by its index
    const Movie* GetMovie(const int i) const {
        if (i >= 0 && i < movieCount) {
            return movies[i];
        }
        return nullptr;
    }

    // Method to get the count of movies
    int getMovieCount() {
        return static_cast<int>(movieCount);
    }

    // Method to set the count of movies
    void setMovieCount(int count) {
        movieCount = count;
    }

    // Method to search movies by title
    void SearchMoviesByTitle(const string& title) {
        movies.Resize(0);
        movies.Resize(20);
        movieCount = 0;
        Get("/3/search/movie", { {"query", title}, {"api_key", "2e249f4b252a6376a7495d987c7ea0e6"} });
        DisplayMovies(1);
    }

    // Method to get upcoming trending movies
    void TrendingMovies(int page) {
        movies.Resize(0);
        movies.Resize(20);
        movieCount = 0;
        Get("/3/movie/upcoming", { {"page", to_string(page)}, {"api_key", "2e249f4b252a6376a7495d987c7ea0e6"} });
    }

    // Method to sort movies by title
    void SortMoviesByTitle(int page, bool ascending) {
        TrendingMovies(page);
        BubbleSort<CompareTitle>(ascending);
        DisplayMovies(page);
    }

    // Method to sort movies by popularity
    void SortMoviesByPopularity(int page, bool ascending) {
        TrendingMovies(page);
        BubbleSort<ComparePopularity>(ascending);
        DisplayMovies(page);
    }

    // Method to get a movie by its ID
    const Movie* GetMovieById(int id) const {
        for (int i = 0; i < movieCount; ++i) {
            if (movies[i]->id == id) {
                return movies[i];
            }
        }
        return nullptr; // Return nullptr if movie with the given ID is not found
    }

    // Overloaded [] operator to access movies by index
    Movie*& operator[](int index) {
        return movies[index];
    }

    // Method to display movies
    void DisplayMovies(int page) {
        cout << "--------------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "|      ID      |                         Title                        |  Release Date  |  Popularity  |  Vote Average  |  Vote Count  |\n";
        cout << "--------------------------------------------------------------------------------------------------------------------------------------\n";

        for (int i = 0; i < movieCount; ++i) {
            const Movie* movie = movies[i];
            cout << "|  " << setw(10) << movie->id
                << "  |  " << setw(50) << movie->title
                << "  |  " << setw(12) << movie->release_date
                << "  |  " << setw(11) << movie->popularity
                << "  |  " << setw(13) << movie->vote_average
                << "  |  " << setw(11) << movie->vote_count << "  |\n";
        }

        cout << "--------------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "*************************   Page: " << page << " **********************************\n\n\n";
    }

protected:

    // Method to parse received data
    virtual void Data(const char* data, const unsigned int size) override {
        try {
            string jsonData(data, size);
            json jsonParsedData = json::parse(jsonData);

            if (jsonParsedData.find("results") != jsonParsedData.end()) {
                // Extract "results" array
                const auto& results = jsonParsedData["results"];

                // Iterate through "results" array and populate movies vector
                for (const auto& result : results) {
                    Movie* movie = new Movie;
                    movie->overview = result["overview"].is_null() ? "" : result["overview"];
                    movie->popularity = result["popularity"];
                    movie->id = result["id"];
                    movie->vote_average = result["vote_average"];
                    movie->vote_count = result["vote_count"];
                    movie->poster_path = result["poster_path"].is_null() ? "" : result["poster_path"];
                    movie->release_date = result["release_date"].is_null() ? "" : result["release_date"];
                    movie->title = result["title"].is_null() ? "" : result["title"];
                    movies[static_cast<int>(movieCount++)] = movie;
                }
            }
        }
        catch (const json::parse_error& e) {
            cout << "Error parsing JSON data: " << e.what() << endl;
        }
    }

    // Method called at the start of receiving data
    virtual void StartOfData() override {}

    // Method called at the end of receiving data
    virtual void EndOfData() override {}
};

void ReadFromFile(MovieClient& mc, const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        mc.setMovieCount(0);
        json jsonData;
        file >> jsonData;
        const auto& moviesData = jsonData["movies"];
        for (const auto& movieData : moviesData) {
            Movie* movie = new Movie;
            movie->id = movieData["id"];
            movie->overview = movieData["overview"];
            movie->popularity = movieData["popularity"];
            movie->poster_path = movieData["poster_path"];
            movie->release_date = movieData["release_date"];
            movie->title = movieData["title"];
            movie->vote_average = movieData["vote_average"];
            movie->vote_count = movieData["vote_count"];
            mc[mc.getMovieCount()] = movie;
            mc.setMovieCount(mc.getMovieCount() +1);
        }
        mc.DisplayMovies(1);
        file.close();
    }
    else {
        cout << "Unable to open file: " << filename << endl;
    }
}

// Function to write data from movie client to a file
void WriteToFile(MovieClient& mc, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        json jsonData;
        jsonData["count"] = mc.getMovieCount();
        json moviesData = json::array();
        for (int i = 0; i < mc.getMovieCount(); ++i) {
            const Movie* movie = mc.GetMovie(i);
            json movieData;
            movieData["id"] = movie->id;
            movieData["overview"] = movie->overview;
            movieData["popularity"] = movie->popularity;
            movieData["poster_path"] = movie->poster_path;
            movieData["release_date"] = movie->release_date;
            movieData["title"] = movie->title;
            movieData["vote_average"] = movie->vote_average;
            movieData["vote_count"] = movie->vote_count;
            moviesData.push_back(movieData);
        }
        jsonData["movies"] = moviesData;
        file << std::setw(4) << jsonData << std::endl;
        file.close();
        cout << "Data written to file: " << filename << endl;
    }
    else {
        cout << "Unable to open file: " << filename << endl;
    }
}

// Function to handle user interaction in the sub-menu
void UserInteractionSubMenu(MovieClient& mc, bool pagination, int page, int prevQuery) {

    const int ARROW_UP = 72;
    const int ARROW_DOWN = 80;

    cout << endl << endl << "Please choose below options: " << endl;

    if (pagination) {
        cout << "Enter an Up arrow key for previous page" << endl;
        cout << "Enter a Down arrow key for next page" << endl;
    }
    cout << "Press v to view for poster image of specific movie" << endl;
    cout << "Press r to read for overview of specific movie" << endl;
    cout << "press Any other key to return main menu" << endl;

    char key = _getch();

    if (key == 'r') {
        cout << "Select ID to read overview: ";
        string indexChar;
        getline(cin, indexChar);

        int index = stoi(indexChar);

        const Movie* movie = mc.GetMovieById(index);

        if (movie != nullptr) {
            cout << "Overview of movie " << movie->title << endl << movie->overview << endl;
        }
        else {
            cout << "Movie not found." << endl;
        }

        UserInteractionSubMenu(mc, pagination, page, prevQuery);
    }

    if (key == 'v') {
        cout << "Select ID to view image: ";
        string indexChar;
        getline(cin, indexChar);

        int index = stoi(indexChar);

        const Movie* movie = mc.GetMovieById(index);

        if (movie != nullptr) {
            string path = "https://image.tmdb.org/t/p/original/" + movie->poster_path;
            ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        else {
            cout << "Movie not found." << endl;
        }

        UserInteractionSubMenu(mc, pagination, page, prevQuery);
    }

    if (pagination == true) {

        int arrowKey = static_cast<int>(key);
        int nextChar = _getch();

        if (nextChar == ARROW_UP || nextChar == ARROW_DOWN) {

            if (nextChar == ARROW_UP) page--;
            if (nextChar == ARROW_DOWN) page++;

            if (page <= 0) page = 1;

            switch (prevQuery) {
            case 2:
                mc.TrendingMovies(page);
                mc.DisplayMovies(page);
                break;
            case 3:
                cout << "Sorting Movies By Title (Ascending)..." << endl;
                mc.SortMoviesByTitle(page, true);
                break;
            case 4:
                cout << "Sorting Movies By Title (Descending)..." << endl;
                mc.SortMoviesByTitle(page, false);
                break;
            case 5:
                cout << "Sorting Movies By Popularity (Ascending)..." << endl;
                mc.SortMoviesByPopularity(page, true);
                break;
            case 6:
                cout << "Sorting Movies By Popularity (Descending)..." << endl;
                mc.SortMoviesByPopularity(page, false);
                break;
            }
            UserInteractionSubMenu(mc, pagination, page, prevQuery);
        }
    }
}

// Function to handle user interaction in the main menu
void UserInteractionMainMenu(MovieClient& mc) {

    while (true) {
        cout << "\n\nWelcome to my TMDB app" << endl;
        cout << "Please choose below options: " << endl;
        cout << "1. Search Movie Based on Title" << endl;
        cout << "2. List of Top Rated Movies" << endl;
        cout << "3. Sort Movies By Title (Ascending)" << endl;
        cout << "4. Sort Movies By Title (Descending)" << endl;
        cout << "5. Sort Movies By Popularity (Ascending)" << endl;
        cout << "6. Sort Movies By Popularity (Descending)" << endl;
        cout << "7. Read Data from File (data.json)" << endl;
        cout << "8. Write Data to File (output.json)" << endl;
        cout << "9. Exit" << endl;

        string queryStr;
        getline(cin, queryStr);

        int query;

        try {
            query = stoi(queryStr);
        }
        catch (const std::invalid_argument& e) {
            cout << "Invalid argument: " << e.what() << endl;
            continue;
        }
        catch (const std::out_of_range& e) {
            cout << "Out of range error: " << e.what() << endl;
            continue;
        }

        switch (query) {
        case 1: {
            string title;
            cout << "Please enter keyword for search movie" << endl;
            getline(cin, title);
            mc.SearchMoviesByTitle(title);
            UserInteractionSubMenu(mc, false, 1, 1);
            break;
        }
        case 2:
            mc.TrendingMovies(1);
            mc.DisplayMovies(1);
            UserInteractionSubMenu(mc, true, 1, 2);
            break;
        case 3:
            cout << "Sorting Movies By Title (Ascending)..." << endl;
            mc.SortMoviesByTitle(1, true);
            UserInteractionSubMenu(mc, true, 1, 3);
            break;
        case 4:
            cout << "Sorting Movies By Title (Descending)..." << endl;
            mc.SortMoviesByTitle(1, false);
            UserInteractionSubMenu(mc, true, 1, 4);
            break;
        case 5:
            cout << "Sorting Movies By Popularity (Ascending)..." << endl;
            mc.SortMoviesByPopularity(1, true);
            UserInteractionSubMenu(mc, true, 1, 5);
            break;
        case 6:
            cout << "Sorting Movies By Popularity (Descending)..." << endl;
            mc.SortMoviesByPopularity(1, false);
            UserInteractionSubMenu(mc, true, 1, 6);
            break;
        case 7:
            cout << "Reading Data from File (data.json)..." << endl;
            ReadFromFile(mc, "data.json");
            break;
        case 8:
            cout << "Writing Data to File (output.json)..." << endl;
            WriteToFile(mc, "output.json");
            break;
        case 9:
            cout << "Exiting TMDB app" << endl;
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main(int argc, char* argv[])
{
    // Leave this, as it will tell you if you have any memory leaks.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MovieClient mc;
    mc.Connect("api.themoviedb.org");

    UserInteractionMainMenu(mc);

}
