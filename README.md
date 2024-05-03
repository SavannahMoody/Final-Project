# TMDB Client Application

This is a simple client application that interacts with The Movie Database (TMDB) API to search for movies, display movie details, and perform various sorting operations.

## Prerequisites

Before running this application, ensure you have the following dependencies installed:

- C++ compiler (e.g., GCC, Clang)
- shlwapi.h (for ShellExecuteA function)
- json.hpp (for JSON parsing)
- httpClient.h

## Functionality

ReadFromFile

- Description: Reads movie data from a JSON file and populates the MovieClient object with the parsed data.
- Parameters:
  - MovieClient& mc: Reference to the MovieClient object to populate.
  - const string& filename: Path to the JSON file containing movie data.
- Return Type: void

WriteToFile

- Description: Writes movie data from the MovieClient object to a JSON file.
- Parameters:
  - MovieClient& mc: Reference to the MovieClient object containing movie data.
  - const string& filename: Path to the output JSON file.
- Return Type: void

UserInteractionSubMenu

- Description: Handles user interaction in the sub-menu, allowing users to view movie details, navigate pages, and perform actions.
- Parameters:
  - MovieClient& mc: Reference to the MovieClient object.
  - bool pagination: Flag indicating whether pagination is enabled.
  - int page: Current page number.
  - int prevQuery: Previous query identifier.
- Return Type: void

UserInteractionMainMenu

- Description: Handles user interaction in the main menu, allowing users to perform various operations such as searching, sorting, and reading/writing data.
- Parameters:
  - MovieClient& mc: Reference to the MovieClient object.
- Return Type: void

## Classes and Methods

### `Movie`

- **Description**: Represents a movie object with various attributes such as ID, overview, popularity, poster path, release date, title, vote average, and vote count.
- **Attributes**:
  - `id`: Unique identifier for the movie.
  - `overview`: Brief description of the movie.
  - `popularity`: Popularity score of the movie.
  - `poster_path`: Path to the movie's poster image.
  - `release_date`: Release date of the movie.
  - `title`: Title of the movie.
  - `vote_average`: Average rating of the movie.
  - `vote_count`: Number of votes for the movie.
- **Methods**:
  - `friend ostream& operator<<(ostream& os, const Movie& movie)`: Overloaded stream insertion operator to facilitate easy printing of movie objects.

### `CompareTitle`

- **Description**: Comparator struct to compare movies by title.
- **Methods**:
  - `bool operator()(const Movie* m1, const Movie* m2) const`: Overloaded function to compare two movie objects by their titles.

### `ComparePopularity`

- **Description**: Comparator struct to compare movies by popularity.
- **Methods**:
  - `bool operator()(const Movie* m1, const Movie* m2) const`: Overloaded function to compare two movie objects by their popularity scores.

### `DynamicArray<T>`

- **Description**: Template class for a dynamic array.
- **Attributes**:
  - `arr`: Pointer to the array elements.
  - `size`: Size of the array.
- **Methods**:
  - `DynamicArray(int initialSize = 20)`: Constructor to initialize the dynamic array with an initial size.
  - `~DynamicArray()`: Destructor to deallocate memory.
  - `void Resize(int newSize)`: Method to resize the array.
  - `T& operator[](int index)`: Overloaded subscript operator to access elements by index.
  - `const T& operator[](int index) const`: Overloaded const subscript operator for accessing elements by index.

### `MovieClient`

- **Description**: Represents a client to interact with the TMDB API.
- **Attributes**:
  - `movies`: Dynamic array to store movie objects.
  - `movieCount`: Number of movies.
- **Methods**:
  - `MovieClient()`: Constructor.
  - `~MovieClient()`: Destructor.
  - `template<typename T> void BubbleSort(bool ascending)`: Template method to perform bubble sort on movies.
  - `const Movie* GetMovie(const int i) const`: Method to get a movie by its index.
  - `int getMovieCount()`: Method to get the count of movies.
  - `void setMovieCount(int count)`: Method to set the count of movies.
  - `void SearchMoviesByTitle(const string& title)`: Method to search movies by title.
  - `void TrendingMovies(int page)`: Method to get upcoming trending movies.
  - `void SortMoviesByTitle(int page, bool ascending)`: Method to sort movies by title.
  - `void SortMoviesByPopularity(int page, bool ascending)`: Method to sort movies by popularity.
  - `const Movie* GetMovieById(int id) const`: Method to get a movie by its ID.
  - `Movie*& operator[](int index)`: Overloaded subscript operator to access movies by index.
  - `void DisplayMovies(int page)`: Method to display movies.
  - `virtual void Data(const char* data, const unsigned int size) override`: Method to parse received data.
  - `virtual void StartOfData() override`: Method called at the start of receiving data.
  - `virtual void EndOfData() override`: Method called at the end of receiving data.
  - `void DeleteMemory()`: Method to delete memory allocated to movies.

### `ReadFromFile` and `WriteToFile`

- **Description**: Functions to read data from a file and write data to a file, respectively.
- **Parameters**:
  - `MovieClient& mc`: Reference to the MovieClient object.
  - `const string& filename`: Name of the file to read from or write to.

### `UserInteractionSubMenu` and `UserInteractionMainMenu`

- **Description**: Functions to handle user interaction in the sub-menu and main menu, respectively.
- **Parameters**:
  - `MovieClient& mc`: Reference to the MovieClient object.
  - `bool pagination`: Flag indicating whether pagination is enabled.
  - `int page`: Current page number.
  - `int prevQuery`: Previous user query.

## Contributing

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.

## Acknowledgments

- This application utilizes the TMDB API for movie data.
- The JSON parsing is handled by nlohmann/json.
