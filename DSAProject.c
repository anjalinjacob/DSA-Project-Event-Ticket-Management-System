#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_EVENTS 5
#define MAX_WAITLIST 5
#define VENUE_NODES 5

int global_booking_id = 1001;

// 1. Structures
typedef struct {
    int booking_id;
    char name[50];
} WaitlistItem;

typedef struct {
    int id;
    char name[50];
    int available_tickets;
    
    WaitlistItem waitlist[MAX_WAITLIST];
    int q_front;
    int q_rear;
} Event;

Event events[MAX_EVENTS] = {
    {1, "Code Sprint", 2, {}, -1, -1},
    {2, "AI Guest Lecture", 0, {}, -1, -1}, 
    {3, "Web Dev Workshop", 1, {}, -1, -1},
    {4, "Data Science Seminar", 3, {}, -1, -1},
    {5, "Cybersecurity Workshop", 5, {}, -1, -1}
};

typedef struct Participant {
    int booking_id;
    char name[50];
    int event_id;
    struct Participant* next;
} Participant;

Participant* head = NULL;

int venue[VENUE_NODES][VENUE_NODES] = {
    {0, 1, 1, 0, 0}, 
    {1, 0, 0, 1, 0}, 
    {1, 0, 0, 1, 1}, 
    {0, 1, 1, 0, 1}, 
    {0, 0, 1, 1, 0}
};

char* node_names[] = {"Entrance","Main Stage","Workshop Hall","Food Court","Exit"};

//2. Waitlist Queue Functions
void enqueue(Event* ev, int b_id, char* name) {
    if (ev->q_rear == MAX_WAITLIST - 1) {
        printf("\n=> FAILED: Waitlist is completely full!\n");
        return;
    }
    if (ev->q_front == -1) {
        ev->q_front = 0;
    }
    ev->q_rear++;
    ev->waitlist[ev->q_rear].booking_id = b_id;
    strcpy(ev->waitlist[ev->q_rear].name, name);   
    printf("\n=> ALERT: Tickets sold out. Added to waitlist!\n");
    printf("=> Your Registration ID is: %d\n", b_id);
}

WaitlistItem dequeue(Event* ev) {
    WaitlistItem item = {-1, ""}; 
    if (ev->q_front == -1) {
        return item;
    }   
    item = ev->waitlist[ev->q_front];
    ev->q_front++; 
    if (ev->q_front > ev->q_rear) {
        ev->q_front = -1;
        ev->q_rear = -1;
    }    
    return item;
}
//3.Linked List Functions
void confirmBooking(int e_id, int b_id, char* name) {
    Participant* new_node = (Participant*)malloc(sizeof(Participant));
    new_node->booking_id = b_id;
    strcpy(new_node->name, name);
    new_node->event_id = e_id;
    new_node->next = head;
    head = new_node;
}

void displayEvents() {
    printf("\n--- Available Events ---\n");
    for (int i = 0; i < MAX_EVENTS; i++) {
        int waitlist_count = (events[i].q_front == -1) ? 0 : (events[i].q_rear - events[i].q_front + 1);        
        printf("ID: %d | %s | Tickets Left: %d | Waitlist Queue: %d/%d\n", 
               events[i].id, events[i].name, events[i].available_tickets, waitlist_count, MAX_WAITLIST);
    }
}

void bookTicket() {
    int e_id;
    char p_name[50];    
    displayEvents();
    printf("\nEnter Event ID to book: ");
    scanf("%d", &e_id);
    getchar();  
    int event_index = -1;
    for(int i = 0; i < MAX_EVENTS; i++) {
        if(events[i].id == e_id) event_index = i;
    }   
    if (event_index == -1) {
        printf("Invalid Event ID.\n");
        return;
    }   
    printf("Enter Participant Name: ");
    fgets(p_name, 50, stdin);
    p_name[strcspn(p_name, "\n")] = 0;
    int assigned_id = global_booking_id++;
    if (events[event_index].available_tickets > 0) {
        events[event_index].available_tickets--;
        confirmBooking(e_id, assigned_id, p_name); 
        printf("\n=> SUCCESS: Ticket booked! Your Booking ID is: %d\n", assigned_id);
    } else {
        enqueue(&events[event_index], assigned_id, p_name); 
    }
}

void cancelTicket() {
    int b_id;
    printf("\nEnter Booking ID to cancel: ");
    scanf("%d", &b_id);   
    Participant* curr = head;
    Participant* prev = NULL;
    bool found = false;   
    while (curr != NULL) {
        if (curr->booking_id == b_id) {
            found = true;
            break;
        }
        prev = curr;
        curr = curr->next;
    }    
    if (!found) {
        printf("Booking ID %d not found in confirmed tickets.\n", b_id);
        return;
    }   
    int ev_id = curr->event_id;
    if (prev == NULL) head = curr->next;
    else prev->next = curr->next;    
    printf("\n=> CANCELLED: Ticket for '%s' (ID: %d) has been cancelled.\n", curr->name, b_id);
    free(curr);   
    int event_index = -1;
    for(int i = 0; i < MAX_EVENTS; i++) {
        if(events[i].id == ev_id) event_index = i;
    }   
    if (events[event_index].q_front != -1) {
        WaitlistItem next_person = dequeue(&events[event_index]);       
        confirmBooking(ev_id, next_person.booking_id, next_person.name);        
        printf("=> UPDATE: Waitlisted person '%s' (ID: %d) has automatically received the ticket!\n", 
               next_person.name, next_person.booking_id);
    } else {
        events[event_index].available_tickets++;
    }
}
//4. Searching algorithm
void searchParticipant() {
    int search_id;
    printf("\nEnter Booking ID to search: ");
    scanf("%d", &search_id);
    Participant* temp = head;
    while (temp != NULL) {
        if (temp->booking_id == search_id) {
            printf("\n=> FOUND: %s is CONFIRMED for Event ID %d\n", temp->name, temp->event_id);
            return;
        }
        temp = temp->next;
    }
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (events[i].q_front != -1) {
            for (int j = events[i].q_front; j <= events[i].q_rear; j++) {
                if (events[i].waitlist[j].booking_id == search_id) {
                    printf("\n=> FOUND: %s is on the WAITLIST for Event ID %d\n", events[i].waitlist[j].name, events[i].id);
                    return;
                }
            }
        }
    }
    
    printf("\n=> NOT FOUND: No record found for Booking ID %d.\n", search_id);
}
//5. Sorting algorithm
void sortEventsByTickets() {
    for (int i = 0; i < MAX_EVENTS - 1; i++) {
        int max_idx = i;
        for (int j = i + 1; j < MAX_EVENTS; j++) {
            if (events[j].available_tickets > events[max_idx].available_tickets) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            Event temp = events[i];
            events[i] = events[max_idx];
            events[max_idx] = temp;
        }
    }    
    printf("\nEvents sorted by ticket availability (descending order)\n");
    displayEvents();
}
//6. Graph Traversal (BFS)
void venueBFS(int start, int destination) {
    int visited[VENUE_NODES] = {0};
    int queue[VENUE_NODES];
    int q_front = 0, q_rear = 0;
    printf("\nPath:\n");
    queue[q_rear++] = start;
    visited[start] = 1;
    while (q_front < q_rear) {
        int current = queue[q_front++];
        printf("[%s] ", node_names[current]);        
        if (current == destination) {
            return;
        }
        printf("-> ");
        for (int i = 0; i < VENUE_NODES; i++) {
            if (venue[current][i] == 1 && !visited[i]) {
                queue[q_rear++] = i;
                visited[i] = 1;
            }
        }
    }
}

int main() {
    int choice;
    do {
        printf("\n==================================\n");
        printf("    EVENT TICKET MANAGEMENT\n");
        printf("==================================\n");
        printf("1. Display Available Events\n");
        printf("2. Book a Ticket\n");
        printf("3. Cancel a Ticket\n");
        printf("4. Search Participant (by ID)\n");
        printf("5. Navigate Venue (BFS Graph)\n");
        printf("6. Sort Events by Availability\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayEvents(); break;
            case 2: bookTicket(); break;
            case 3: cancelTicket(); break;
            case 4: searchParticipant(); break;
            case 5: {
                int start, dest;
                printf("\n0:Entrance, 1:Stage, 2:Workshop, 3:Food, 4:Exit\n");
                printf("Enter Start Node (0-4): ");
                scanf("%d", &start);
                printf("Enter Destination Node (0-4): ");
                scanf("%d", &dest);
                if (start >= 0 && start <= 4 && dest >= 0 && dest <= 4) venueBFS(start, dest);
                else printf("Invalid nodes.\n");
                break;
            }
            case 6: sortEventsByTickets(); break;
            case 7: printf("Exiting System...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 7);

    return 0;
}
