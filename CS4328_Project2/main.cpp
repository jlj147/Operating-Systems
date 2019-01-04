//This is the current version
#include<iostream>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<cstdlib>
#include <algorithm>
#include <queue>

using namespace std;

//Function Prototypes
void *create_thread(void *);
int dealerf();
void player1f(int);
void player2f(int);
void player3f(int);

//Function prototypes of operations
void displayHand(int,int,int);
int matchCheck(int,int);
int drawCard(int);
void discard(int,int);
void exitRound();
void displayDeck();
void displayWinner1();
void displayWinner2();
void displayWinner3();
void displayLoser1();
void displayLoser2();
void displayLoser3();

//Global Structs
queue<int> DeckOfCards; //Deck of cards that players will draw from
vector<int> TempDeck; //Cards to be shuffled and then assigned to main deck

//Global Variables
int match; //Variable to hold the card match flag
int numRounds=0; //Variable to hold the number of rounds to be played
int roundNum=0; //Variable to hold the round currently being played
int card1=0; //Variable to hold the card given to player 1 by the dealer
int card2=0; ///Variable to hold the card given to player 1 by the dealer
int card3=0; //Variable to hold the card given to player 1 by the dealer
int newCard1=0; //Variable to hold the card drawn from the deck by player 1
int newCard2=0; //Variable to hold the card drawn from the deck by player 2
int newCard3=0; //Variable to hold the card drawn from the deck by player 3
int winStatus = 0; //Variable to hold the winstatus

//Mutex and condition variabled
pthread_mutex_t myLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t myLock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t myLock3 = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    int *d=0; //Dummy variable to pass when creating thread
    
    pthread_t dealer,player1,player2,player3;
    
    pthread_create(&dealer,NULL,create_thread,(void *)d);
    pthread_create(&player1,NULL,create_thread,(void *)d);
    pthread_create(&player2,NULL,create_thread,(void *)d);
    pthread_create(&player3,NULL,create_thread,(void *)d);
    
    pthread_join(dealer,NULL);
    pthread_join(player1,NULL);
    pthread_join(player2,NULL);
    pthread_join(player3,NULL);
    
    dealerf();
    
    return 0;
}

void *create_thread(void *ptr)
{
    cout << "Thread Created " << endl;
    return NULL;
}

//DEALER FUNCTION
//*****************************************************************************
int dealerf()
{
    numRounds++;
    match = 0;
    roundNum++;
    
    //Create a temporary deck of 52 cards
    for (int i=0;i<4;i++)
    {
        for (int j=1;j<=13;j++)
        {
            TempDeck.push_back(j);
        }
    }
    
    //Shuffle the temp deck a random number of times
    cout << "DEALER: shuffle" << endl;
    cout << endl;
    srand(static_cast<unsigned int>(time(NULL)));
    int shuffles = rand()%20;
    
    for(int i=0;i<shuffles;i++)
    {
        random_shuffle(TempDeck.begin(),TempDeck.end());
    }
    
    //Push the shuffled temp deck into the DeckOfCards queue
    for (int i=0; i<TempDeck.size();i++)
    {
        DeckOfCards.push(TempDeck[i]);
    }
    
    TempDeck.clear();
    
    if(numRounds<4)
    {
        card1=DeckOfCards.front();
        DeckOfCards.pop();
        
        card2=DeckOfCards.front();
        DeckOfCards.pop();
        
        card3=DeckOfCards.front();
        DeckOfCards.pop();
    }
    
    if (roundNum==1)
    {
        cout << "ROUND" << roundNum << ":" << endl;
        player1f(card1);
    }
    
    if (roundNum==2)
    {
        cout << "ROUND" << roundNum << ":" << endl;
        player2f(card2);
    }
    
    if (roundNum==3)
    {
        cout << "ROUND" << roundNum << ":" << endl;
        player3f(card3);
    }
    
    return 0;
}

//PLAYER FUNCTIONS
//*****************************************************************************
void player1f(int x)
{
    int player = 1; //Variable to designate the player hand being displayed
    
    pthread_mutex_lock(&myLock);
    
    do
    {
        cout <<  "Player 1: hand " << x << endl;
        newCard1=drawCard(player);
        displayHand(player,x,newCard1);
        match = matchCheck(x,newCard1);
        
        if (match==0)
        {
            discard(player,newCard1);
            cout <<  "Player 1: hand " << x << endl;
            displayDeck();
            cout << endl;
            pthread_mutex_unlock(&myLock);
            player2f(card2);
        }
        else
        {
            cout << "Player " << player << ": wins" << endl;
            displayWinner1();
            displayLoser2();
            displayLoser3();
            exitRound();
            dealerf();
        }
        pthread_mutex_unlock(&myLock);
        
    }while (numRounds<4);
}

void player2f(int x)
{
    int player = 2;
    
    //pthread_mutex_lock(&myLock);
    pthread_mutex_lock(&myLock2);
    
    do
    {
        cout << "Player 2: hand " << x << endl;
        newCard2=drawCard(player);
        displayHand(player,x,newCard2);
        match = matchCheck(x,newCard2);
        
        if (match==0)
        {
            discard(player,newCard2);
            cout <<  "Player 2: hand " << x << endl;
            displayDeck();
            cout << endl;
            pthread_mutex_unlock(&myLock2);
            pthread_mutex_unlock(&myLock3);
            player3f(card3);
        }
        else
        {
            cout << "Player " << player << ": wins" << endl;
            displayLoser1();
            displayWinner2();
            displayLoser3();
            exitRound();
            dealerf();
        }
        //pthread_mutex_unlock(&myLock);
        pthread_mutex_unlock(&myLock2);
        
    }while (numRounds<4);
    
}

void player3f(int x)
{
    int player = 3;
    
    //pthread_mutex_lock(&myLock);
    pthread_mutex_lock(&myLock3);
    
    do
    {
        cout << "Player 3: hand " << x << endl;
        newCard3=drawCard(player);
        displayHand(player,x,newCard3);
        match = matchCheck(x,newCard3);
        
        if (match==0)
        {
            discard(player,newCard3);
            cout <<  "Player 3: hand " << x << endl;
            displayDeck();
            cout << endl;
            pthread_mutex_unlock(&myLock3);
            pthread_mutex_unlock(&myLock2);
            pthread_mutex_unlock(&myLock);
            player1f(card1);
        }
        else
        {
            cout << "Player " << player << ": wins" << endl;
            displayLoser1();
            displayLoser2();
            displayWinner3();
            exitRound();
            dealerf();
        }
        //pthread_mutex_unlock(&myLock);
        pthread_mutex_unlock(&myLock3);
        
    }while (numRounds<4);
    
}

//OPERATION FUNCTIONS
//*****************************************************************************
int drawCard(int player)
{
    int newCard;
    newCard = DeckOfCards.front();
    cout << "PLAYER " << player << ": draws " << newCard << endl;
    DeckOfCards.pop();
    return newCard;
}

void displayHand(int player,int x,int y)
{
    cout << "PLAYER " << player << ": hand " << x << " " << y << endl;
}

int matchCheck(int x,int y)
{
    if (x==y)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void discard(int player, int y)
{
    cout << "PLAYER " << player << ": discards " << y << endl;
    DeckOfCards.push(y);
}

void exitRound()
{
    cout << "PLAYER 1: exits round" << endl;
    cout << "PLAYER 2: exits round" << endl;
    cout << "PLAYER 3: exits round" << endl;
    
    while(!DeckOfCards.empty())
    {
        DeckOfCards.pop();
    }
}

void displayDeck()
{
    int x=0;
    
    TempDeck.clear();
    
    cout << "DECK: ";
    while (!DeckOfCards.empty())
    {
        x=DeckOfCards.front();
        DeckOfCards.pop();
        TempDeck.push_back(x);
        cout << x << " ";
    }
    
    for (int i=0; i<=51;i++)
    {
        DeckOfCards.push(TempDeck[i]);
    }
    
    TempDeck.clear();
    cout << endl;
}

void displayWinner1()
{
    cout << endl;
    cout << "Player 1:"<< endl;
    cout << "HAND " << card1 << " " << newCard1 << endl;
    cout << "WIN yes" << endl;
    displayDeck();
}

void displayWinner2()
{
    cout << endl;
    cout << "Player 2:"<< endl;
    cout << "HAND " << card2 << " " << newCard2 << endl;
    cout << "WIN yes" << endl;
    displayDeck();
}

void displayWinner3()
{
    cout << endl;
    cout << "Player 3:"<< endl;
    cout << "HAND " << card3 << " " << newCard3 << endl;
    cout << "WIN yes" << endl;
    displayDeck();
}

void displayLoser1()
{
    cout << endl;
    cout << "Player 1:"<< endl;
    cout << "HAND " << card1 << " " << newCard1 << endl;
    cout << "WIN no" << endl;
    displayDeck();
}
void displayLoser2()
{
    cout << endl;
    cout << "Player 2:"<< endl;
    cout << "HAND " << card2 << " " << newCard2 << endl;
    cout << "WIN no" << endl;
    displayDeck();
}

void displayLoser3()
{
    cout << endl;
    cout << "Player 3:"<< endl;
    cout << "HAND " << card3 << " " << newCard3 << endl;
    cout << "WIN no" << endl;
    displayDeck();
}




