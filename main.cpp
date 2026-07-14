#include "main.h"

//take the ascii of each character and add them
int calculateHash(string key, int tableSize)
{
	int sum = 0;
	for (int i = 0; i < key.length(); i++)
		sum += key[i];
	return sum % tableSize;
}

string cleanString(string str)
{
	if (str.empty())
		return str;

	string cleaned = "";
	for (int i = 0; i < str.length(); i++)
	{
		unsigned char c = (unsigned char)str[i];
		if (c >= 32 && c <= 126)
			cleaned += char(c);
		else if (c == 0xC2 || c == 0xC3)
			// Skip UTF-8 continuation bytes that might be part of multi-byte characters
			// We'll skip these to avoid encoding issues
			continue;
	}
	return cleaned;
}

string trim(string str)
{
	str = cleanString(str);

	if (str.empty())
		return str;

	//this is to remove trailing and leading white spaces, marking the start and end of the actual string 
	//" \t Hello World    \t  \n"
	//the start is then marked at 3 and end at 20 
	int start = 0;
	while (start < str.length() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r'))
		start++;

	int end = str.length();
	if (end > 0)
		end--;

	while (end >= start && end < str.length() && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r'))
	{
		if (end == 0)
			break;
		end--;
	}

	if (start > end)
		return "";

	return str.substr(start, end - start + 1);
}

genreNode::genreNode(string passedGenre)
{
	genre = passedGenre;
	next = nullptr;
}

movieNode::movieNode()
{
	title = "";
	genres = nullptr;
	actors = nullptr;
	director = "";
	rating = 0.0;
	releaseYear = 0;
}

movieNode::movieNode(string passedTitle, genreNode* passedGenres, ActorNode* passedActors, string passedDirector, float passedRating, int passedReleaseYear)
{
	title = passedTitle;
	genres = passedGenres;
	actors = passedActors;
	director = passedDirector;
	rating = passedRating;
	releaseYear = passedReleaseYear;
}

//this is so that we can make a list using movieNodes since they only store data
//this list is used in actorNode
movieListNode::movieListNode(movieNode* m)
{
	movie = m;
	next = nullptr;
}

ActorNode::ActorNode(string passedName)
{
	actorName = passedName;
	next = nullptr;
	moviesHead = nullptr;
	movieCount = 0;
}

ActorNode::~ActorNode()
{
	//delete movieListNode chain for this actor
	movieListNode* current = moviesHead;
	while (current != nullptr)
	{
		movieListNode* temp = current;
		current = current->next;
		delete temp;
	}
}

//insertion at head so its O(1)
void ActorNode::addMovie(movieNode* passedMovie)
{
	//check if movie already exists in the list
	movieListNode* current = moviesHead;
	while (current != nullptr)
	{
		if (current->movie == passedMovie)
			return;
		current = current->next;
	}

	movieListNode* newNode = new movieListNode(passedMovie);
	newNode->next = moviesHead;
	moviesHead = newNode;
	movieCount++;
}

void ActorNode::getMovies()
{
	cout << "Movies featuring " << actorName << ":" << endl;

	movieListNode* current = moviesHead;
	while (current != nullptr)
	{
		cout << "- " << current->movie->title << " (" << current->movie->releaseYear << ")" << endl;
		current = current->next;
	}
}

void ActorNode::getCoActors()
{
	cout << "Co-actors of " << actorName << ":" << endl;

	string* coActors = new string[1000];
	int coActorCount = 0;

	//traverse the list of movies stored under that actor
	//in those movies traverse the list of actors stored
	//maintain another array of strings that tracks all the co-actors
	movieListNode* movieCurrent = moviesHead;
	while (movieCurrent != nullptr)
	{
		ActorNode* actorCurrent = movieCurrent->movie->actors;
		while (actorCurrent != nullptr)
		{
			if (actorCurrent->actorName != actorName)
			{
				bool alreadyExists = false;
				for (int i = 0; i < coActorCount; i++)
				{
					if (coActors[i] == actorCurrent->actorName)
					{
						alreadyExists = true;
						break;
					}
				}
				if (!alreadyExists && coActorCount < 1000)
					coActors[coActorCount++] = actorCurrent->actorName;
			}
			actorCurrent = actorCurrent->next;
		}
		movieCurrent = movieCurrent->next;
	}

	for (int i = 0; i < coActorCount; i++)
		cout << "- " << coActors[i] << endl;

	if (coActorCount == 0)
		cout << "No co-actors found" << endl;

	delete[] coActors;
}

void ActorNode::displayInfo()
{
	cout << "Actor: " << actorName << endl;
	cout << "Total movies: " << movieCount << endl;
	getMovies();
	cout << endl;
}

//this is again another list of actors, but used in the hash table for chaining
//the actor list in movies has a different next
// here it has a different next (as this groups similar key-value pairs, the other shows actors in that movie)
ActorHashNode::ActorHashNode(ActorNode* actorPtr)
{
	actor = actorPtr;
	next = nullptr;
}

//same reasoning as ActorHashNode 
GenreHashNode::GenreHashNode(string name)
{
	genreName = name;
	movies = nullptr;
	next = nullptr;
}

int ActorHashTable::hashFunction(string key)
{
	return calculateHash(key, tableSize);
}

ActorHashTable::ActorHashTable()
{
	tableSize = 1009;
	table = new ActorHashNode * [tableSize];
	for (int i = 0; i < tableSize; i++)
		table[i] = nullptr;
}

//insertion at head for actorHashNode linkedlist
void ActorHashTable::insert(string actorName, ActorNode* actor)
{
	int index = hashFunction(actorName);
	ActorHashNode* newNode = new ActorHashNode(actor);
	newNode->next = table[index];
	table[index] = newNode;
}

//we get the index in the array then traverse the chain at that hash value index
ActorNode* ActorHashTable::search(string actorName)
{
	int index = hashFunction(actorName);
	ActorHashNode* current = table[index];
	while (current != nullptr)
	{
		if (current->actor->actorName == actorName)
			return current->actor;
		current = current->next;
	}
	return nullptr;
}

ActorNode* ActorHashTable::getOrCreate(string actorName)
{
	ActorNode* actor = search(actorName);
	if (actor == nullptr)
	{
		actor = new ActorNode(actorName);
		insert(actorName, actor);
	}
	return actor;
}

ActorHashTable::~ActorHashTable()
{
	//delete hash nodes and their actor nodes and movie list chains

	//this is for each bucket in the hash table
	for (int i = 0; i < tableSize; i++)
	{
		ActorHashNode* current = table[i];
		//linked list deletion for the chain at that index
		while (current != nullptr)
		{
			ActorHashNode* temp = current;
			current = current->next;
			//ActorNode contain pointers to movieNodes but the movieNodes are different data
			//ActorNode destructor will delete its movieListNode chain
			if (temp->actor)
				//ActorNode destructor deletes movieListNode chain
				delete temp->actor;
			delete temp;
		}
	}
	delete[] table;
}

int GenreHashTable::hashFunction(string key)
{
	return calculateHash(key, tableSize);
}

GenreHashTable::GenreHashTable()
{
	tableSize = 101;
	table = new GenreHashNode * [tableSize];
	for (int i = 0; i < tableSize; i++)
		table[i] = nullptr;
}

//same as actorHashTable
void GenreHashTable::insert(string genreName)
{
	int index = hashFunction(genreName);
	GenreHashNode* current = table[index];
	while (current != nullptr)
	{
		if (current->genreName == genreName)
			return;
		current = current->next;
	}
	//insertion at head on the list stored at that index or array
	GenreHashNode* newNode = new GenreHashNode(genreName);
	newNode->next = table[index];
	table[index] = newNode;
}

//same as actorHashTable
movieListNode* GenreHashTable::search(string genreName)
{
	int index = hashFunction(genreName);
	GenreHashNode* current = table[index];
	while (current != nullptr)
	{
		if (current->genreName == genreName)
			return current->movies;
		current = current->next;
	}
	return nullptr;
}

//except actorHashNode here 1 genre has multiple movies
//search for that genre and add that movie to the list of movies in that genre
//check for duplicates to prevent same movie being added twice
void GenreHashTable::addMovieToGenre(string genreName, movieNode* movie)
{
	int index = hashFunction(genreName);
	GenreHashNode* current = table[index];
	while (current != nullptr)
	{
		if (current->genreName == genreName)
		{
			//check if movie already exists in this genre's list
			movieListNode* movieCurrent = current->movies;
			while (movieCurrent != nullptr)
			{
				if (movieCurrent->movie == movie)
				{
					//movie already exists in this genre, don't add duplicate
					return;
				}
				movieCurrent = movieCurrent->next;
			}

			//movie doesn't exist, add it
			//insertion at head, movies attribute in movieListNode is the head pointer of the movie list in that genre
			movieListNode* newNode = new movieListNode(movie);
			newNode->next = current->movies;
			current->movies = newNode;
			return;
		}
		//not found keep moving
		current = current->next;
	}
	//doesn't exist so make new
	insert(genreName);
	addMovieToGenre(genreName, movie);
}

GenreHashTable::~GenreHashTable()
{
	//delete hash nodes and their movie list chains

	//this is for each bucket in the hash table
	for (int i = 0; i < tableSize; i++)
	{
		GenreHashNode* current = table[i];
		//linked list deletion for the chain at that index
		while (current != nullptr)
		{
			GenreHashNode* temp = current;
			current = current->next;
			//movieListNode contain pointers to movieNodes that must also be deleted
			//extract the movieNode and delete that first, then delete the movieListNode
			movieListNode* movieCurrent = temp->movies;
			int nodeCount = 0;
			while (movieCurrent != nullptr)
			{
				movieListNode* movieTemp = movieCurrent;
				movieCurrent = movieCurrent->next;
				delete movieTemp;
				nodeCount++;
			}
			delete temp;
		}
	}
	delete[] table;
}

TreeNode::TreeNode(movieNode* passedMovie)
{
	movie = passedMovie;
	height = 1;
	left = nullptr;
	right = nullptr;
}

MovieAVLTree::MovieAVLTree()
{
	root = nullptr;
}

int MovieAVLTree::max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

int MovieAVLTree::getHeight(TreeNode* node)
{
	if (!node)
		return 0;
	else
		return node->height;
}

int MovieAVLTree::getBalanceFactor(TreeNode* node)
{
	if (!node)
		return 0;
	else
		return (getHeight(node->left) - getHeight(node->right));
}

void MovieAVLTree::updateHeight(TreeNode* node)
{
	if (!node)
		return;

	node->height = 1 + max(getHeight(node->left), getHeight(node->right));
}

TreeNode* MovieAVLTree::rightRotate(TreeNode* y)
{
	TreeNode* x = y->left;
	TreeNode* T2 = x->right;

	x->right = y;
	y->left = T2;

	updateHeight(y);
	updateHeight(x);

	return x;
}

TreeNode* MovieAVLTree::leftRotate(TreeNode* x)
{
	TreeNode* y = x->right;
	TreeNode* T2 = y->left;

	y->left = x;
	x->right = T2;

	updateHeight(x);
	updateHeight(y);

	return y;
}

TreeNode* MovieAVLTree::insert(TreeNode* node, movieNode* movie)
{
	if (!node)
		return new TreeNode(movie);

	string movieTitle = trim(movie->title);
	string nodeTitle = trim(node->movie->title);

	if (movieTitle < nodeTitle)
		node->left = insert(node->left, movie);
	else if (movieTitle > nodeTitle)
		node->right = insert(node->right, movie);
	else
		return node;

	updateHeight(node);

	int balanceFactor = getBalanceFactor(node);

	if (balanceFactor > 1 && movieTitle < trim(node->left->movie->title))
		return rightRotate(node);

	if (balanceFactor < -1 && movieTitle > trim(node->right->movie->title))
		return leftRotate(node);

	if (balanceFactor > 1 && movieTitle > trim(node->left->movie->title))
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (balanceFactor < -1 && movieTitle < trim(node->right->movie->title))
	{
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

TreeNode* MovieAVLTree::minValueNode(TreeNode* node)
{
	if (!node)
		return nullptr;

	TreeNode* current = node;

	while (current->left != nullptr)
		current = current->left;
	return current;
}

TreeNode* MovieAVLTree::deleteNode(TreeNode* node, string title)
{
	if (!node)
		return node;

	string searchTitle = trim(title);
	string nodeTitle = trim(node->movie->title);

	if (searchTitle < nodeTitle)
		node->left = deleteNode(node->left, title);
	else if (searchTitle > nodeTitle)
		node->right = deleteNode(node->right, title);
	else
	{
		if (!node->left || !node->right)
		{
			TreeNode* temp;
			if (node->left)
				temp = node->left;
			else
				temp = node->right;

			if (!temp)
			{
				temp = node;
				node = nullptr;
			}
			else
				*node = *temp;
			delete temp;
		}
		else
		{
			TreeNode* temp = minValueNode(node->right);
			node->movie = temp->movie;

			node->right = deleteNode(node->right, temp->movie->title);
		}
	}

	if (!node)
		return node;

	updateHeight(node);

	int balanceFactor = getBalanceFactor(node);

	if (balanceFactor > 1 && getBalanceFactor(node->left) >= 0)
		return rightRotate(node);

	if (balanceFactor > 1 && getBalanceFactor(node->left) < 0)
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (balanceFactor < -1 && getBalanceFactor(node->right) <= 0)
		return leftRotate(node);

	if (balanceFactor < -1 && getBalanceFactor(node->right) > 0)
	{
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

void MovieAVLTree::inorder(TreeNode* node)
{
	if (!node)
		return;

	inorder(node->left);
	cout << node->movie->title << " (" << node->movie->releaseYear << ")\n";
	inorder(node->right);
}

TreeNode* MovieAVLTree::search(TreeNode* node, string title)
{
	if (!node)
		return nullptr;

	string nodeTitle = trim(node->movie->title);
	string searchTitle = trim(title);

	if (searchTitle == nodeTitle)
		return node;

	if (searchTitle < nodeTitle)
		return search(node->left, searchTitle);
	else
		return search(node->right, searchTitle);
}

void MovieAVLTree::insertMovie(movieNode* movie)
{
	root = insert(root, movie);
}

void MovieAVLTree::deleteMovie(string title)
{
	root = deleteNode(root, title);
}

void MovieAVLTree::printInOrder()
{
	inorder(root);
}

movieNode* MovieAVLTree::findMovie(string title)
{
	string searchTitle = trim(title);
	TreeNode* result = search(root, searchTitle);
	if (!result)
		return nullptr;

	return result->movie;
}

TreeNode* MovieAVLTree::getRoot()
{
	return root;
}

void MovieAVLTree::collectMovies(movieNode** movies, int& index)
{
	collectMoviesHelper(root, movies, index);
}

int MovieAVLTree::getMovieCount()
{
	return getMovieCountHelper(root);
}

MovieAVLTree::~MovieAVLTree()
{
	destroyTree(root);
}

void MovieAVLTree::destroyTree(TreeNode* node)
{
	if (node == nullptr)
		return;

	//post order
	destroyTree(node->left);
	destroyTree(node->right);

	//delete the movieNode and its genre and actor linked lists
	if (node->movie)
	{
		//delete genre linked list (genreNode chain)
		genreNode* genreCurrent = node->movie->genres;
		while (genreCurrent != nullptr)
		{
			genreNode* genreTemp = genreCurrent;
			genreCurrent = genreCurrent->next;
			delete genreTemp;
		}
		//delete actor linked list (ActorNode chain)
		ActorNode* actorCurrent = node->movie->actors;
		while (actorCurrent != nullptr)
		{
			ActorNode* actorTemp = actorCurrent;
			actorCurrent = actorCurrent->next;
			//ActorNode in AVL tree contain pointers to movieNodes but movieNodes are owned by hash table
			//so only delete the ActorNode, not its moviesHead since the hash table will delete it
			delete actorTemp;
		}
		//delete the movieNode itself (this is the actual owner of the movie data)
		delete node->movie;
	}
	//delete the tree node itself
	delete node;
}

//traverse in-order and store all movies in an array
//pass index as reference so array index is consistent across recursion calls (like in lab task of AVL accuracy)
void MovieAVLTree::collectMoviesHelper(TreeNode* node, movieNode** movies, int& index)
{
	if (!node)
		return;

	collectMoviesHelper(node->left, movies, index);
	movies[index++] = node->movie;
	collectMoviesHelper(node->right, movies, index);
}

int MovieAVLTree::getMovieCountHelper(TreeNode* node)
{
	if (!node)
		return 0;

	return 1 + getMovieCountHelper(node->left) + getMovieCountHelper(node->right);
}

GraphNode::GraphNode(int index)
{
	vertexIndex = index;
	next = nullptr;
}

StackNode::StackNode(int value)
{
	data = value;
	next = nullptr;
}

Stack::Stack()
{
	top = nullptr;
}

Stack::~Stack()
{
	while (!isEmpty())
		pop();
}

void Stack::push(int value)
{
	StackNode* newNode = new StackNode(value);
	newNode->next = top;
	top = newNode;
}

int Stack::pop()
{
	if (isEmpty())
		return -1;

	StackNode* temp = top;
	int value = top->data;
	top = top->next;
	delete temp;
	return value;
}

int Stack::peek()
{
	if (isEmpty())
		return -1;
	return top->data;
}

bool Stack::isEmpty()
{
	return top == nullptr;
}

QueueNode::QueueNode(int value)
{
	data = value;
	next = nullptr;
}

Queue::Queue()
{
	front = nullptr;
	rear = nullptr;
}

Queue::~Queue()
{
	while (!isEmpty())
		dequeue();
}

void Queue::enqueue(int value)
{
	QueueNode* newNode = new QueueNode(value);

	if (rear == nullptr)
		front = rear = newNode;
	else
	{
		rear->next = newNode;
		rear = newNode;
	}
}

int Queue::dequeue()
{
	if (isEmpty())
		return -1;

	QueueNode* temp = front;
	int value = front->data;
	front = front->next;

	if (front == nullptr)
		rear = nullptr;

	delete temp;
	return value;
}

int Queue::peek()
{
	if (isEmpty())
		return -1;

	return front->data;
}

bool Queue::isEmpty()
{
	return front == nullptr;
}


bool Graph::shareActors(movieNode* m1, movieNode* m2)
{
	//traverse both until the actors get the same movie, else return false
	ActorNode* actor1 = m1->actors;
	while (actor1 != nullptr)
	{
		ActorNode* actor2 = m2->actors;
		while (actor2 != nullptr)
		{
			if (actor1->actorName == actor2->actorName)
				return true;
			actor2 = actor2->next;
		}
		actor1 = actor1->next;
	}
	return false;
}


bool Graph::shareGenres(movieNode* m1, movieNode* m2)
{
	//traverse both until the genres get the same movie, else return false
	genreNode* genre1 = m1->genres;
	while (genre1 != nullptr)
	{
		genreNode* genre2 = m2->genres;
		while (genre2 != nullptr)
		{
			if (genre1->genre == genre2->genre)
				return true;
			genre2 = genre2->next;
		}
		genre1 = genre1->next;
	}
	return false;
}

Graph::Graph(int maxVertices)
{
	//allocate arrays for adjacency lists and movie pointers, set everything to null initially
	V = maxVertices;
	adj = new GraphNode * [V];
	movies = new movieNode * [V];
	movieCount = 0;

	for (int i = 0; i < V; i++)
	{
		adj[i] = nullptr;
		movies[i] = nullptr;
	}
}

void Graph::addEdge(int u, int v)
{
	GraphNode* newNode = new GraphNode(v);
	newNode->next = adj[u];
	adj[u] = newNode;
}

void Graph::buildGraph(MovieAVLTree* tree)
{
	movieCount = 0;
	tree->collectMovies(movies, movieCount);

	//for each pair of movies, check if they share actors (only actor connections, not genres)
	//this creates a sparser graph where BFS and DFS will show different paths
	for (int i = 0; i < movieCount; i++)
	{
		for (int j = i + 1; j < movieCount; j++)
		{
			if (shareActors(movies[i], movies[j]))
			{
				addEdge(i, j);
				addEdge(j, i);
			}
		}
	}
}

int Graph::getMovieCount()
{
	return movieCount;
}

int Graph::getMovieIndex(string title)
{
	title = trim(title);
	for (int i = 0; i < movieCount; i++)
	{
		if (movies[i])
		{
			string movieTitle = trim(movies[i]->title);
			if (movieTitle == title)
				return i;
		}
	}
	return -1;
}

int Graph::getMovieIndexByPointer(movieNode* movie)
{
	if (!movie)
		return -1;

	for (int i = 0; i < movieCount; i++)
	{
		if (movies[i] == movie)
			return i;
	}
	return -1;
}

bool Graph::bfsPath(int src, int dest)
{
	//use queue for BFS and mark visited nodes and keep track of parent for reconstructing path
	bool* visited = new bool[V];
	int* parent = new int[V];
	for (int i = 0; i < V; i++)
	{
		visited[i] = false;
		parent[i] = -1;
	}

	//create queue for BFS traversal
	Queue queue;

	//mark source as visited and enqueue it
	visited[src] = true;
	queue.enqueue(src);

	while (!queue.isEmpty())
	{
		int current = queue.dequeue();

		if (current == dest)
			break;

		//for each adjacent vertex
		GraphNode* temp = adj[current];
		while (temp != nullptr)
		{
			int neighbor = temp->vertexIndex;
			//if neighbor is unvisited
			if (visited[neighbor] == false)
			{
				//mark as visited and enqueue
				visited[neighbor] = true;
				//keep track of parent for path reconstruction
				parent[neighbor] = current;
				queue.enqueue(neighbor);
			}
			temp = temp->next;
		}
		//current vertex is now fully processed
	}

	//check if path exists
	if (visited[dest] == false)
	{
		cout << "No path found using BFS" << endl;
		delete[] visited;
		delete[] parent;
		return false;
	}

	//reconstruct path from destination to source using parent pointers
	int* path = new int[V];
	int pathLen = 0;
	int node = dest;
	while (node != -1)
	{
		path[pathLen++] = node;
		node = parent[node];
	}

	//print path from source to destination
	cout << "BFS Shortest Path: ";
	for (int i = pathLen - 1; i >= 0; i--)
	{
		cout << movies[path[i]]->title;
		if (i > 0) cout << " -> ";
	}
	cout << endl;

	delete[] visited;
	delete[] parent;
	delete[] path;
	return true;
}

//DFS path finding using stack
void Graph::findAndPrintPath(int src, int dest)
{
	//use stack for DFS to find path from source to destination, reconstruct using parent pointers
	bool* visited = new bool[V];
	int* parent = new int[V];
	for (int i = 0; i < V; i++)
	{
		visited[i] = false;
		parent[i] = -1;
	}

	Stack stack;

	//mark source as visited and push it
	visited[src] = true;
	stack.push(src);

	bool found = false;
	while (!stack.isEmpty())
	{
		int current = stack.pop();

		if (current == dest)
		{
			found = true;
			break;
		}

		//for each adjacent vertex
		//first collect all unvisited neighbors
		int* neighbors = new int[V];
		int neighborCount = 0;
		GraphNode* temp = adj[current];
		while (temp != nullptr)
		{
			int neighbor = temp->vertexIndex;

			if (!visited[neighbor])
				neighbors[neighborCount++] = neighbor;
			temp = temp->next;
		}
		//push neighbors in reverse order
		for (int i = neighborCount - 1; i >= 0; i--)
		{
			visited[neighbors[i]] = true;
			//keep track of parent for path reconstruction
			parent[neighbors[i]] = current;
			stack.push(neighbors[i]);
		}
		delete[] neighbors;
	}

	if (!found)
	{
		cout << "No path exists from " << movies[src]->title << " to " << movies[dest]->title << endl;
		delete[] visited;
		delete[] parent;
		return;
	}

	//reconstruct path from destination to source using parent pointers
	int* path = new int[V];
	int pathLen = 0;
	int node = dest;
	while (node != -1)
	{
		path[pathLen++] = node;
		node = parent[node];
	}

	cout << "DFS Path from " << movies[src]->title << " to " << movies[dest]->title << ": ";
	for (int i = pathLen - 1; i >= 0; i--)
	{
		cout << movies[path[i]]->title;
		if (i > 0)
			cout << " -> ";
	}
	cout << endl;
	cout << "(Note: DFS may not find the shortest path - use BFS for shortest path)" << endl;

	delete[] visited;
	delete[] parent;
	delete[] path;
}

//BFS recommendations algorithm
void Graph::bfsRecommendations(int src, int maxDepth)
{
	//perform BFS with distance tracking to find movies within given depth
	bool* visited = new bool[V];
	int* distance = new int[V];
	for (int i = 0; i < V; i++)
	{
		visited[i] = false;
		distance[i] = -1;
	}

	Queue queue;

	//mark source as visited and enqueue
	visited[src] = true;
	distance[src] = 0;
	queue.enqueue(src);

	cout << "BFS Recommendations for " << movies[src]->title << ":" << endl;
	cout << "(BFS explores level by level - distance 1 = directly connected, distance 2 = connected through one movie)" << endl;

	//BFS traversal to discover vertices by distance, stop at maxDepth
	while (!queue.isEmpty())
	{
		int current = queue.dequeue();
		int currentDepth = distance[current];

		//stop exploring if we've reached maxDepth
		if (currentDepth >= maxDepth)
			continue;

		//for each adjacent vertex
		GraphNode* temp = adj[current];
		while (temp != nullptr)
		{
			int neighbor = temp->vertexIndex;
			//only explore neighbors if within maxDepth limit
			if (visited[neighbor] == false && currentDepth + 1 <= maxDepth)
			{
				visited[neighbor] = true;
				distance[neighbor] = currentDepth + 1;
				queue.enqueue(neighbor);
			}
			temp = temp->next;
		}
	}

	//display recommendations in order of distance, closest first
	int count = 0;
	for (int d = 1; d <= maxDepth; d++)
	{
		for (int i = 0; i < V; i++)
		{
			if (distance[i] == d)
			{
				cout << "- " << movies[i]->title << " (distance: " << d << ")" << endl;
				count++;
			}
		}
	}

	if (count == 0)
		cout << "No recommendations found within distance " << maxDepth << endl;
	else
		cout << "\nTotal recommendations found: " << count << endl;

	delete[] visited;
	delete[] distance;
}

//DFS recommendations using stack
void Graph::getDFSRecommendations(int src, int maxDepth)
{
	//perform DFS with stack and distance tracking to explore up to given depth
	bool* visited = new bool[V];
	int* distance = new int[V];
	for (int i = 0; i < V; i++)
	{
		visited[i] = false;
		distance[i] = -1;
	}

	Stack stack;

	//mark source as visited and push it
	visited[src] = true;
	distance[src] = 0;
	stack.push(src);

	cout << "DFS Recommendations for " << movies[src]->title << ":" << endl;
	cout << "(DFS explores one connection path deeply - showing movies along that path)" << endl;

	//DFS traversal to explore deeply, stop at maxDepth
	int count = 0;
	while (!stack.isEmpty())
	{
		int current = stack.pop();
		int currentDepth = distance[current];

		//stop exploring if we've exceeded maxDepth
		if (currentDepth > maxDepth)
			continue;

		//display recommendation, skip source at depth 0
		if (currentDepth > 0 && currentDepth <= maxDepth)
		{
			cout << "- " << movies[current]->title << " (depth: " << currentDepth << ")" << endl;
			count++;
		}

		//stop exploring neighbors if we've reached maxDepth
		if (currentDepth >= maxDepth)
			continue;

		//for each adjacent vertex (push in reverse order to explore left-to-right)
		//collect all neighbors first
		int* neighbors = new int[V];
		int neighborCount = 0;
		GraphNode* temp = adj[current];
		while (temp != nullptr)
		{
			int neighbor = temp->vertexIndex;
			//only add neighbors if within maxDepth limit
			if (!visited[neighbor] && currentDepth + 1 <= maxDepth)
				neighbors[neighborCount++] = neighbor;
			temp = temp->next;
		}

		//push neighbors in reverse order
		for (int i = neighborCount - 1; i >= 0; i--)
		{
			int neighbor = neighbors[i];
			visited[neighbor] = true;
			distance[neighbor] = currentDepth + 1;
			stack.push(neighbor);
		}
		delete[] neighbors;
	}

	if (count == 0)
		cout << "No recommendations found within depth " << maxDepth << endl;
	else
		cout << "\nTotal recommendations found: " << count << endl;

	delete[] visited;
	delete[] distance;
}

Graph::~Graph()
{
	//for each vertex, traverse adjacency list and delete all GraphNode objects
	for (int i = 0; i < V; i++)
	{
		GraphNode* current = adj[i];
		while (current != nullptr)
		{
			GraphNode* temp = current;
			current = current->next;
			delete temp;
		}
	}
	//delete the adjacency list array
	delete[] adj;
	//delete movies array, contains pointers owned by AVL tree
	delete[] movies;
}

void parseCSV(string filename, MovieAVLTree* tree, ActorHashTable* actorTable, GenreHashTable* genreTable)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		cout << "Failed to open file: " << filename << endl;
		return;
	}

	string line;
	getline(file, line);

	int movieCount = 0;
	while (getline(file, line))
	{
		stringstream ss(line);
		string token;
		string fields[28];
		int fieldIndex = 0;

		bool inQuotes = false;
		string currentField = "";

		for (int i = 0; i < line.length(); i++)
		{
			char c = line[i];
			if (c == '"')
				inQuotes = !inQuotes;
			else if (c == ',' && !inQuotes)
			{
				fields[fieldIndex++] = currentField;
				currentField = "";
			}
			else
				currentField += c;
		}
		//there are 28 total fields but 27 commas
		if (fieldIndex < 27)
			fields[fieldIndex] = currentField;

		//this removes the incomplete movie data (means theres no director name, actor names and genre)
		if (fieldIndex < 11)
			continue;

		string director = trim(fields[1]);
		string actor2 = trim(fields[6]);
		string genresStr = trim(fields[9]);
		string actor1 = trim(fields[10]);
		string title = trim(fields[11]);
		title = cleanString(title);
		title = trim(title);
		string actor3 = trim(fields[14]);
		string yearStr = trim(fields[23]);
		string ratingStr = trim(fields[25]);

		if (title.empty() || yearStr.empty() || ratingStr.empty())
			continue;

		int year = 0;
		float rating = 0.0;
		year = stoi(yearStr);
		rating = stof(ratingStr);

		genreNode* genresHead = nullptr;
		stringstream genreStream(genresStr);
		string genreToken;
		while (getline(genreStream, genreToken, '|'))
		{
			genreToken = trim(genreToken);
			if (!genreToken.empty())
			{
				genreNode* newGenre = new genreNode(genreToken);
				newGenre->next = genresHead;
				genresHead = newGenre;
				genreTable->insert(genreToken);
			}
		}

		ActorNode* actorsHead = nullptr;
		if (!actor1.empty())
		{
			ActorNode* newActor = new ActorNode(actor1);
			newActor->next = actorsHead;
			actorsHead = newActor;
		}
		if (!actor2.empty())
		{
			ActorNode* newActor = new ActorNode(actor2);
			newActor->next = actorsHead;
			actorsHead = newActor;
		}
		if (!actor3.empty())
		{
			ActorNode* newActor = new ActorNode(actor3);
			newActor->next = actorsHead;
			actorsHead = newActor;
		}

		title = trim(title);

		//check if movie already exists in tree to prevent duplicate movies
		movieNode* existingMovie = tree->findMovie(title);
		if (existingMovie != nullptr)
		{
			//movie already exists, skip this row and clean up allocated memory
			ActorNode* actorCurrent = actorsHead;
			while (actorCurrent != nullptr)
			{
				ActorNode* temp = actorCurrent;
				actorCurrent = actorCurrent->next;
				delete temp;
			}
			genreNode* genreCurrent = genresHead;
			while (genreCurrent != nullptr)
			{
				genreNode* temp = genreCurrent;
				genreCurrent = genreCurrent->next;
				delete temp;
			}
			continue;
		}

		movieNode* movie = new movieNode(title, genresHead, actorsHead, director, rating, year);
		tree->insertMovie(movie);

		//prevent duplicate genres within the same movie
		string* uniqueGenres = new string[10];
		int uniqueGenreCount = 0;
		genreNode* genreCurrent = genresHead;
		while (genreCurrent != nullptr)
		{
			string genreName = trim(genreCurrent->genre);
			bool isDuplicate = false;

			//check if this genre is already in our unique list
			for (int i = 0; i < uniqueGenreCount; i++)
			{
				if (uniqueGenres[i] == genreName)
				{
					isDuplicate = true;
					break;
				}
			}

			//only add if not duplicate
			if (!isDuplicate && !genreName.empty())
			{
				uniqueGenres[uniqueGenreCount++] = genreName;
				genreTable->addMovieToGenre(genreName, movie);
			}
			genreCurrent = genreCurrent->next;
		}
		delete[] uniqueGenres;

		//prevent duplicate actors within the same movie (actor1, actor2, actor3 might be same)
		string* uniqueActors = new string[3];
		int uniqueActorCount = 0;
		ActorNode* actorCurrent = actorsHead;
		while (actorCurrent != nullptr)
		{
			string actorName = trim(actorCurrent->actorName);
			bool isDuplicate = false;

			//check if this actor is already in our unique list
			for (int i = 0; i < uniqueActorCount; i++)
			{
				if (uniqueActors[i] == actorName)
				{
					isDuplicate = true;
					break;
				}
			}

			//only add if not duplicate
			if (!isDuplicate && !actorName.empty())
			{
				uniqueActors[uniqueActorCount++] = actorName;
				ActorNode* actorNode = actorTable->getOrCreate(actorName);
				actorNode->addMovie(movie);
			}
			actorCurrent = actorCurrent->next;
		}
		delete[] uniqueActors;

		movieCount++;
	}

	file.close();
	cout << "Loaded " << movieCount << " movies from CSV" << endl;
}

void searchByTitle(MovieAVLTree* tree, string title)
{
	string searchTitle = trim(title);
	movieNode* movie = tree->findMovie(searchTitle);

	if (movie)
	{
		cout << "Found: " << movie->title << " (" << movie->releaseYear << ")" << endl;
		cout << "Director: " << movie->director << endl;
		cout << "Rating: " << movie->rating << endl;
		cout << "Actors: ";

		ActorNode* actor = movie->actors;
		while (actor)
		{
			cout << actor->actorName;
			actor = actor->next;
			if (actor)
				cout << ", ";
		}

		cout << endl;
		cout << "Genres: ";
		genreNode* genre = movie->genres;
		while (genre)
		{
			cout << genre->genre;
			genre = genre->next;
			if (genre)
				cout << ", ";
		}

		cout << endl;
	}
	else
		cout << "Movie not found: " << title << endl;
}

void searchByActor(ActorHashTable* actorTable, string actorName)
{
	ActorNode* actor = actorTable->search(actorName);
	if (actor)
		actor->displayInfo();
	else
		cout << "Actor not found: " << actorName << endl;
}

void searchByGenre(GenreHashTable* genreTable, string genreName)
{
	movieListNode* movies = genreTable->search(genreName);
	if (movies)
	{
		cout << "Movies in genre \"" << genreName << "\":" << endl;
		movieListNode* current = movies;
		int count = 0;
		while (current != nullptr && count < 20)
		{
			cout << "- " << current->movie->title << " (" << current->movie->releaseYear << ")" << endl;
			current = current->next;
			count++;
		}
	}
	else
		cout << "Genre not found: " << genreName << endl;
}

void searchByYearHelper(TreeNode* node, int year)
{
	if (!node)
		return;

	searchByYearHelper(node->left, year);

	if (node->movie->releaseYear == year)
		cout << "- " << node->movie->title << " (Rating: " << node->movie->rating << ")" << endl;

	searchByYearHelper(node->right, year);
}

void searchByYear(MovieAVLTree* tree, int year)
{
	cout << "Movies from year " << year << ":" << endl;
	searchByYearHelper(tree->getRoot(), year);
}

void searchByRatingHelper(TreeNode* node, float minRating, float maxRating)
{
	if (!node)
		return;

	searchByRatingHelper(node->left, minRating, maxRating);

	if (node->movie->rating >= minRating && node->movie->rating <= maxRating)
		cout << "- " << node->movie->title << " (" << node->movie->releaseYear << ") - Rating: " << node->movie->rating << endl;

	searchByRatingHelper(node->right, minRating, maxRating);
}

void searchByRatingRange(MovieAVLTree* tree, float minRating, float maxRating)
{
	cout << "Movies with rating between " << minRating << " and " << maxRating << ":" << endl;
	searchByRatingHelper(tree->getRoot(), minRating, maxRating);
}

int main()
{
	cout << "=== Movies Data Manager ===" << endl;
	cout << "Loading data from CSV..." << endl << endl;

	MovieAVLTree* tree = new MovieAVLTree();
	ActorHashTable* actorTable = new ActorHashTable();
	GenreHashTable* genreTable = new GenreHashTable();

	parseCSV("movie_metadata.csv", tree, actorTable, genreTable);

	cout << endl << "=== Statistics ===" << endl;
	int movieCount = tree->getMovieCount();
	cout << "Total movies loaded: " << movieCount << endl;
	cout << endl;

	cout << "=== Building Graph ===" << endl;
	Graph* graph = new Graph(movieCount);
	graph->buildGraph(tree);
	cout << "Graph built successfully! Data loading complete." << endl << endl;

	bool running = true;
	while (running)
	{
		cout << "\n========== MOVIES DATA MANAGER MENU ==========" << endl;
		cout << "--- SEARCH OPERATIONS ---" << endl;
		cout << "1. Search movie by title" << endl;
		cout << "2. Search movies by actor" << endl;
		cout << "3. Search movies by genre" << endl;
		cout << "4. Search movies by year" << endl;
		cout << "5. Search movies by rating range" << endl;
		cout << "6. Print actor filmography" << endl;
		cout << "7. Get co-actors of an actor" << endl;
		cout << "8. Display all movies" << endl;
		cout << "9. Get movie recommendations (BFS)" << endl;
		cout << "10. Get movie recommendations (DFS)" << endl;
		cout << "11. Find shortest path between two movies (BFS)" << endl;
		cout << "12. Find shortest path between two movies (DFS)" << endl;
		cout << "13. Delete a movie" << endl;
		cout << "14. Rebuild graph" << endl;
		cout << "0. Exit" << endl;
		cout << "==============================================" << endl;
		cout << "Enter your choice (0-14): ";

		int choice;
		cin >> choice;
		cin.ignore();

		if (choice == 1)
		{
			cout << "Enter movie title to search: ";
			string title;
			getline(cin, title);
			searchByTitle(tree, title);
		}
		else if (choice == 2)
		{
			cout << "Enter actor name to search: ";
			string actorName;
			getline(cin, actorName);
			searchByActor(actorTable, actorName);
		}
		else if (choice == 3)
		{
			cout << "Enter genre to search: ";
			string genreName;
			getline(cin, genreName);
			searchByGenre(genreTable, genreName);
		}
		else if (choice == 4)
		{
			cout << "Enter year to search: ";
			int year;
			cin >> year;
			searchByYear(tree, year);
		}
		else if (choice == 5)
		{
			cout << "Enter minimum rating: ";
			float minRating;
			cin >> minRating;
			cout << "Enter maximum rating: ";
			float maxRating;
			cin >> maxRating;
			searchByRatingRange(tree, minRating, maxRating);
		}
		else if (choice == 6)
		{
			cout << "Enter actor name to print filmography: ";
			string actorName;
			getline(cin, actorName);
			ActorNode* actor = actorTable->search(actorName);
			if (actor)
				actor->getMovies();
			else
				cout << "Actor not found: " << actorName << endl;
		}
		else if (choice == 7)
		{
			cout << "Enter actor name to get co-actors: ";
			string actorName;
			getline(cin, actorName);
			ActorNode* actor = actorTable->search(actorName);
			if (actor)
				actor->getCoActors();
			else
				cout << "Actor not found: " << actorName << endl;
		}
		else if (choice == 8)
		{
			cout << "Displaying all movies (in alphabetical order):" << endl;
			cout << "==================================================" << endl;
			tree->printInOrder();
			cout << "\nTotal movies displayed: " << tree->getMovieCount() << endl;
		}
		else if (choice == 9)
		{
			cout << "Enter movie title for BFS recommendations: ";
			string movieTitle;
			getline(cin, movieTitle);

			movieNode* movie = tree->findMovie(movieTitle);

			if (!movie)
				cout << "Movie not found: " << movieTitle << endl;
			else
			{
				int movieIndex = graph->getMovieIndexByPointer(movie);
				if (movieIndex == -1)
				{
					string trimmedTitle = trim(movie->title);
					movieIndex = graph->getMovieIndex(trimmedTitle);
				}

				if (movieIndex != -1)
				{
					cout << "Using movie: " << trim(movie->title) << endl;
					graph->bfsRecommendations(movieIndex, 1);
				}
				else
					cout << "Movie not found in graph." << endl;
			}
		}
		else if (choice == 10)
		{
			cout << "Enter movie title for DFS recommendations: ";
			string movieTitle;
			getline(cin, movieTitle);

			movieNode* movie = tree->findMovie(movieTitle);
			if (!movie)
				cout << "Movie not found: " << movieTitle << endl;
			else
			{
				int movieIndex = graph->getMovieIndexByPointer(movie);
				if (movieIndex == -1)
				{
					string trimmedTitle = trim(movie->title);
					movieIndex = graph->getMovieIndex(trimmedTitle);
				}

				if (movieIndex != -1)
					graph->getDFSRecommendations(movieIndex, 1);
				else
					cout << "Movie not found in graph." << endl;
			}
		}
		else if (choice == 11)
		{
			cout << "Enter first movie title: ";
			string movie1Title;
			getline(cin, movie1Title);

			cout << "Enter second movie title: ";
			string movie2Title;
			getline(cin, movie2Title);

			movieNode* movie1 = tree->findMovie(movie1Title);
			movieNode* movie2 = tree->findMovie(movie2Title);

			if (!movie1 || !movie2)
			{
				if (!movie1)
					cout << "First movie not found: " << movie1Title << endl;

				if (!movie2)
					cout << "Second movie not found: " << movie2Title << endl;
			}
			else
			{
				int movie1Index = graph->getMovieIndexByPointer(movie1);
				if (movie1Index == -1)
				{
					string trimmedTitle1 = trim(movie1->title);
					movie1Index = graph->getMovieIndex(trimmedTitle1);
				}

				int movie2Index = graph->getMovieIndexByPointer(movie2);
				if (movie2Index == -1)
				{
					string trimmedTitle2 = trim(movie2->title);
					movie2Index = graph->getMovieIndex(trimmedTitle2);
				}

				if (movie1Index != -1 && movie2Index != -1)
				{
					cout << "Finding shortest path from '" << trim(movie1->title) << "' to '" << trim(movie2->title) << "'" << endl;
					graph->bfsPath(movie1Index, movie2Index);
				}
				else
					cout << "Cannot find path - one or both movies not found in graph." << endl;
			}
		}
		else if (choice == 12)
		{
			cout << "Enter first movie title: ";
			string movie1Title;
			getline(cin, movie1Title);

			cout << "Enter second movie title: ";
			string movie2Title;
			getline(cin, movie2Title);

			movieNode* movie1 = tree->findMovie(movie1Title);
			movieNode* movie2 = tree->findMovie(movie2Title);

			if (!movie1 || !movie2)
			{
				if (!movie1)
					cout << "First movie not found: " << movie1Title << endl;
				if (!movie2)
					cout << "Second movie not found: " << movie2Title << endl;
			}
			else
			{
				int movie1Index = graph->getMovieIndexByPointer(movie1);
				if (movie1Index == -1)
				{
					string trimmedTitle1 = trim(movie1->title);
					movie1Index = graph->getMovieIndex(trimmedTitle1);
				}

				int movie2Index = graph->getMovieIndexByPointer(movie2);
				if (movie2Index == -1)
				{
					string trimmedTitle2 = trim(movie2->title);
					movie2Index = graph->getMovieIndex(trimmedTitle2);
				}

				if (movie1Index != -1 && movie2Index != -1)
					graph->findAndPrintPath(movie1Index, movie2Index);
				else
					cout << "Cannot find path - one or both movies not found in graph." << endl;
			}
		}
		else if (choice == 13)
		{
			cout << "Enter movie title to delete: ";
			string title;
			getline(cin, title);
			movieNode* movie = tree->findMovie(title);
			if (movie)
			{
				tree->deleteMovie(title);
				cout << "Movie '" << title << "' deleted successfully." << endl;
				cout << "Note: Graph needs to be rebuilt after deletion for accurate recommendations." << endl;
			}
			else
				cout << "Movie not found: " << title << endl;
		}
		else if (choice == 14)
		{
			cout << "Rebuilding graph..." << endl;
			delete graph;
			int movieCount = tree->getMovieCount();
			graph = new Graph(movieCount);
			graph->buildGraph(tree);
			cout << "Graph rebuilt successfully! Total movies in graph: " << movieCount << endl;
		}
		else if (choice == 0)
			running = false;
		else
			cout << "Invalid choice! Please enter a number between 0 and 14." << endl;
	}


	//deletion order is important to avoid doubledelete
	cout << "Deleting graph..." << endl;
	delete graph;
	cout << "Graph deleted" << endl;

	cout << "Deleting genre table..." << endl;
	delete genreTable;
	cout << "Genre table deleted" << endl;

	cout << "Deleting actor table..." << endl;
	delete actorTable;
	cout << "Actor table deleted" << endl;

	cout << "Deleting tree..." << endl;
	delete tree;
	cout << "Tree deleted" << endl;

	return 0;
}