#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// ==========================================
// CARD CLASS
// ==========================================
class Card {
private:
    string suit;
    string rank;
    int value;

public:
    Card(string s, string r, int v) {
        suit = s;
        rank = r;
        value = v;
    }
    int getValue() { return value; }
    string getRank() { return rank; }
    void displayCard() {
        cout << rank << " of " << suit;
    }
};

// ==========================================
// DECK CLASS
// ==========================================
class Deck {
private:
    vector<Card> cards;

public:
    Deck() { 
        createDeck(); 
    }

    void createDeck() {
        cards.clear();
        string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", 
                          "Jack", "Queen", "King", "Ace"};
        int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                cards.push_back(Card(suits[i], ranks[j], values[j]));
            }
        }
    }

    void shuffleDeck() {
        int deckSize = cards.size();
        for (int i = 0; i < deckSize; i++) {
            int random_index = rand() % deckSize;
            swap(cards[i], cards[random_index]);
        }
    }

    Card dealCard() {
        if(cards.empty()) {
            cout << "Deck is empty! Reshuffling..." << endl;
            createDeck();
            shuffleDeck();
        }
        Card topCard = cards.back();
        cards.pop_back();
        return topCard;
    }
};

// ==========================================
// PARTICIPANT CLASS (BASE)
// ==========================================
class Participant {
protected:
    vector<Card> hand;

public:
    void addCard(Card c) {
        hand.push_back(c);
    }

    void clearHand() {
        hand.clear();
    }

    int calculateScore() {
        int score = 0;
        int aces = 0;
        for (int i = 0; i < hand.size(); i++) {
            score += hand[i].getValue();
            if (hand[i].getRank() == "Ace") {
                aces++;
            }
        }
        while (score > 21 && aces > 0) {
            score -= 10;
            aces--;
        }
        return score;
    }

    void displayFullHand() {
        for (int i = 0; i < hand.size(); i++) {
            hand[i].displayCard();
            if (i != hand.size() - 1) cout << " | ";
        }
        cout << " (Score: " << calculateScore() << ")" << endl;
    }
};

// ==========================================
// PLAYER CLASS (DERIVED)
// ==========================================
class Player : public Participant {
private:
    int balance;
    int currentBet;

public:
    Player() {
        balance = 1000;
        currentBet = 0;
    }
    void placeBet(int amount) { currentBet = amount; }
    void winBet() { balance += currentBet; }
    void loseBet() { balance -= currentBet; }
    int getBalance() { return balance; }
};

// ==========================================
// DEALER CLASS (DERIVED)
// ==========================================
class Dealer : public Participant {
public:
    void showPartialHand() {
        cout << "Dealer's Hand: ";
        hand[0].displayCard();
        cout << " | [Hidden Card]" << endl;
    }
};

// ==========================================
// GAME CONTROLLER CLASS
// ==========================================
class Game {
private:
    Deck deck;
    Player player;
    Dealer dealer;

public:
    void startGame() {
        cout << "===================================" << endl;
        cout << "       WELCOME TO BLACKJACK        " << endl;
        cout << "===================================" << endl;
        
        bool playing = true;
        while (playing && player.getBalance() > 0) {
            playRound();
            if (player.getBalance() <= 0) {
                cout << "You are out of money! Game Over." << endl;
                break;
            }
            char choice;
            cout << "\nPlay another round? (y/n): ";
            cin >> choice;
            if (choice == 'n' || choice == 'N') {
                playing = false;
            }
        }
        cout << "Thank you for playing! Final Balance: $" << player.getBalance() << endl;
    }

    void playRound() {
        player.clearHand();
        dealer.clearHand();
        deck.createDeck();
        deck.shuffleDeck();

        cout << "\nCurrent Balance: $" << player.getBalance() << endl;
        int bet = 0;
        while (true) {
            cout << "Enter your bet: $";
            cin >> bet;
            if (bet > 0 && bet <= player.getBalance()) {
                break;
            }
            cout << "Invalid bet. Please enter an amount up to your current balance." << endl;
        }
        player.placeBet(bet);

        // Initial Deal
        player.addCard(deck.dealCard());
        dealer.addCard(deck.dealCard());
        player.addCard(deck.dealCard());
        dealer.addCard(deck.dealCard());

        cout << "\n--- Initial Deal ---" << endl;
        cout << "Your Hand: ";
        player.displayFullHand();
        dealer.showPartialHand();

        // Player Turn
        if (player.calculateScore() == 21) {
            cout << "Blackjack! You win!" << endl;
            player.winBet();
            return;
        }

        bool playerBusted = false;
        while (true) {
            char choice;
            cout << "\nChoose action - (H)it or (S)tand: ";
            cin >> choice;

            if (choice == 'h' || choice == 'H') {
                player.addCard(deck.dealCard());
                cout << "Your Hand: ";
                player.displayFullHand();

                if (player.calculateScore() > 21) {
                    cout << "Bust! Your score exceeded 21." << endl;
                    playerBusted = true;
                    break;
                }
            } else if (choice == 's' || choice == 'S') {
                break;
            } else {
                cout << "Invalid choice." << endl;
            }
        }

        if (playerBusted) {
            player.loseBet();
            return;
        }

        // Dealer Turn
        cout << "\n--- Dealer's Turn ---" << endl;
        cout << "Dealer's Hand: ";
        dealer.displayFullHand();

        while (dealer.calculateScore() < 17) {
            cout << "Dealer hits..." << endl;
            dealer.addCard(deck.dealCard());
            cout << "Dealer's Hand: ";
            dealer.displayFullHand();
        }

        // Determine Winner
        determineWinner();
    }

    void determineWinner() {
        int playerScore = player.calculateScore();
        int dealerScore = dealer.calculateScore();

        cout << "\n--- Final Results ---" << endl;
        cout << "Your Score: " << playerScore << endl;
        cout << "Dealer Score: " << dealerScore << endl;

        if (dealerScore > 21) {
            cout << "Dealer busts! You win!" << endl;
            player.winBet();
        } else if (playerScore > dealerScore) {
            cout << "You have the higher score. You win!" << endl;
            player.winBet();
        } else if (dealerScore > playerScore) {
            cout << "Dealer has the higher score. You lose." << endl;
            player.loseBet();
        } else {
            cout << "It's a tie (Push)!" << endl;
        }
    }
};

// ==========================================
// MAIN EXECUTION
// ==========================================
int main() {
    srand(time(0)); 
    Game blackjack;
    blackjack.startGame();
    return 0;
}