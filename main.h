
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class movieNode;
class ActorNode;

struct genreNode
{
	string genre;
	genreNode* next;
	genreNode(string passedGenre);
};

class movieNode
{
public:
	string title;
	genreNode* genres;
	ActorNode* actors;
	string director;
	float rating;
	int releaseYear;

	movieNode();
	movieNode(string passedTitle, genreNode* passedGenres, ActorNode* passedActors, string passedDirector, float passedRating, int passedReleaseYear);
};

struct movieListNode
{
	movieNode* movie;
	movieListNode* next;
	movieListNode(movieNode* m);
};

class ActorNode
{
public:
	string actorName;
	ActorNode* next;
	movieListNode* moviesHead;
	int movieCount;

	ActorNode(string passedName);
	~ActorNode();
	void addMovie(movieNode* passedMovie);
	void getMovies();
	void getCoActors();
	void displayInfo();
};

struct ActorHashNode
{
	ActorNode* actor;
	ActorHashNode* next;
	ActorHashNode(ActorNode* actorPtr);
};

struct GenreHashNode
{
	string genreName;
	//each genre has multiple movies, so this is to store that list of movies in this genre
	movieListNode* movies;
	GenreHashNode* next;
	GenreHashNode(string name);
};

class ActorHashTable
{
public:
	ActorHashTable();
	~ActorHashTable();
	void insert(string actorName, ActorNode* actor);
	ActorNode* search(string actorName);
	ActorNode* getOrCreate(string actorName);
private:
	int tableSize;
	ActorHashNode** table;
	int hashFunction(string key);
};

class GenreHashTable
{
public:
	GenreHashTable();
	~GenreHashTable();
	void insert(string genreName);
	movieListNode* search(string genreName);
	void addMovieToGenre(string genreName, movieNode* movie);
private:
	int tableSize;
	GenreHashNode** table;
	int hashFunction(string key);
};

struct TreeNode
{
	movieNode* movie;
	int height;
	TreeNode* left;
	TreeNode* right;
	TreeNode(movieNode* passedMovie);
};

struct GraphNode
{
	int vertexIndex;
	GraphNode* next;
	GraphNode(int index);
};

struct StackNode
{
	int data;
	StackNode* next;
	StackNode(int value);
};

struct QueueNode
{
	int data;
	QueueNode* next;
	QueueNode(int value);
};

class MovieAVLTree
{
public:
	MovieAVLTree();
	~MovieAVLTree();
	void insertMovie(movieNode* movie);
	void deleteMovie(string title);
	void printInOrder();
	movieNode* findMovie(string title);
	TreeNode* getRoot();
	void collectMovies(movieNode** movies, int& index);
	int getMovieCount();
private:
	TreeNode* root;
	int max(int a, int b);
	int getHeight(TreeNode* node);
	int getBalanceFactor(TreeNode* node);
	void updateHeight(TreeNode* node);
	TreeNode* rightRotate(TreeNode* y);
	TreeNode* leftRotate(TreeNode* x);
	TreeNode* insert(TreeNode* node, movieNode* movie);
	TreeNode* minValueNode(TreeNode* node);
	TreeNode* deleteNode(TreeNode* node, string title);
	void inorder(TreeNode* node);
	TreeNode* search(TreeNode* node, string title);
	void destroyTree(TreeNode* node);
	void collectMoviesHelper(TreeNode* node, movieNode** movies, int& index);
	int getMovieCountHelper(TreeNode* node);
};

class Stack
{
public:
	Stack();
	~Stack();
	void push(int value);
	int pop();
	int peek();
	bool isEmpty();
private:
	StackNode* top;
};

class Queue
{
public:
	Queue();
	~Queue();
	void enqueue(int value);
	int dequeue();
	int peek();
	bool isEmpty();
private:
	QueueNode* front;
	QueueNode* rear;
};

class Graph
{
public:
	Graph(int maxVertices);
	~Graph();
	void addEdge(int u, int v);
	void buildGraph(MovieAVLTree* tree);
	int getMovieCount();
	int getMovieIndex(string title);
	int getMovieIndexByPointer(movieNode* movie);
	bool bfsPath(int src, int dest);
	void findAndPrintPath(int src, int dest);
	void bfsRecommendations(int src, int maxDepth);
	void getDFSRecommendations(int src, int maxDepth);
private:
	int V;
	GraphNode** adj;
	movieNode** movies;
	int movieCount;
	bool shareActors(movieNode* m1, movieNode* m2);
	bool shareGenres(movieNode* m1, movieNode* m2);
};

int calculateHash(string key, int tableSize);
string cleanString(string str);
string trim(string str);

void parseCSV(string filename, MovieAVLTree* tree, ActorHashTable* actorTable, GenreHashTable* genreTable);

void searchByTitle(MovieAVLTree* tree, string title);
void searchByActor(ActorHashTable* actorTable, string actorName);
void searchByGenre(GenreHashTable* genreTable, string genreName);
void searchByYear(MovieAVLTree* tree, int year);
void searchByRatingRange(MovieAVLTree* tree, float minRating, float maxRating);

void searchByYearHelper(TreeNode* node, int year);
void searchByRatingHelper(TreeNode* node, float minRating, float maxRating);


