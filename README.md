# News Broadcasting Scenario Simulator

## Welcome to the News Broadcasting Scenario Simulator! 
### This project simulates the dynamic environment of a live news broadcast, where multiple stories are produced, categorized, edited, and finally displayed on the screen for the audience. <br>This simulation encapsulates the various stages of news production and distribution, ensuring that stories are delivered to the public in a timely and organized manner.

## Participants
1. **Producer:**
   Each Producer is responsible for generating news stories in the form of strings. These stories are categorized into three types: SPORTS, NEWS, and WEATHER. Each Producer sends its stories to a Dispatcher for further processing. Once all stories are produced, the Producer signals completion with a special message.

Example Stories:
<pre>
Producer 1 NEWS 0
Producer 2 SPORTS 1
Producer 3 WEATHER 2
</pre>
2. **Dispatcher:**
   The Dispatcher acts as the central hub, collecting stories from all Producers. It organizes the stories based on their type (SPORTS, NEWS, WEATHER) and forwards them to the appropriate Co-Editor. The Dispatcher operates continuously, ensuring that stories are sorted and delivered without delay.

3. **Co-Editors:**
   The Co-Editors refine the stories received from the Dispatcher. Each Co-Editor specializes in one type of story (SPORTS, NEWS, WEATHER). After a brief "editing" process, the stories are passed to the Screen Manager for final display.

4. **Screen Manager:**
   The Screen Manager is the final destination for all stories. It displays the stories on the screen in the order they are received. Once all stories have been displayed, the Screen Manager signals the completion of the broadcast.

## System Design
The system is designed with the following components:

**Producer Queues:** Each Producer has its own queue for sending stories to the Dispatcher.<br>
**Dispatcher Queues:** The Dispatcher uses separate queues for each type of story (SPORTS, NEWS, WEATHER).<br>
**Shared Queue:** Co-Editors use a shared queue to send edited stories to the Screen Manager.<br>
**Bounded Buffer:**
The system uses a bounded buffer for the Producer queues and the shared queue between Co-Editors and the Screen Manager. The buffer ensures that the system operates smoothly even under varying loads.<br>
**Configuration File:**
The system is configured using a file that specifies the number of stories each Producer should generate and the size of the queues.

## Example Configuration:
<pre>
PRODUCER 1
30
queue size = 5

PRODUCER 2
25
queue size = 3

PRODUCER 3
16
queue size = 30

Co-Editor queue size = 17
</pre>

## Running the Simulator
To run the simulation, compile the program using the provided Makefile and execute it with the configuration file as an argument:

<pre>
./ex3.out config.txt
</pre>