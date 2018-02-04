//Because my teammates never jump on time
//Yes, I used this as an excuse to play a lot of PUBG
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265
#define GRAVITY_CONST 10.0
#define FALL_VY_CONST 5.0

struct node
{
    double x;
    double y;
    struct node* next;
};
typedef struct node node;

node* make_node(double x, double y)
{
	node* temp = (node*)malloc(sizeof(node));
	temp->x = x;
	temp->y = y;
	return temp;
}

node* make_empty_node()
{
	node* temp = (node*) malloc (sizeof(node));
	return temp;
}

void attach_node(node* current, node* next)
{
	current->next = next;
}

//makes deep copy
node* copy_node(node* original)
{
	node* new_node = (node*) malloc (sizeof(node));
	new_node->x = original->x;
	new_node->y = original->y;
	attach_node(new_node, original->next);
	return new_node;
}

double find_min(double* x, int length)
{
	if(length == 0 || x == NULL)
	{
		printf("Error, bad array was passed to find_min");
		return -1;
	}
	else
	{
		double min = x[0];
		for(int i = 0; i < length; i++)
		{
			if(x[i] < min)
				min = x[i];
		}
		return min;
	}
}

//returns the best path to chute deploy location given that you already jumped
//x,y is distance to chute deploy location (which is at origin)
double path_before_chute(double time, double x, double y,  double dt, double angle, node* location_node)
{
	printf("X: %f, Y: %f, Angle: %f!\n", x, y, angle);
	if(x <= 0 && y <= 0)
	{
		printf("Made it to end\n");
		return time;  // return chute path time
	}		
	else if(y <= 0) //else if(didn't make it to target)
		return 1000000; //TODO: improve the case where you don't make it
	else if(x <= 0)
	{
		//fly straight down
		double temp_v_x = -cos(angle)*GRAVITY_CONST;
		double temp_v_y = -sin(angle)*GRAVITY_CONST - FALL_VY_CONST*dt;
		double temp_x = temp_v_x*dt + x;
		double temp_y = temp_v_y*dt + y;

		node* min_node = make_node(temp_x, temp_y);
		attach_node(location_node, min_node);

		double angle_to_location = PI/2;

		return path_before_chute(time+dt, temp_x, temp_y,  dt, angle_to_location, min_node);
	}

	//Assumption, never want to make angle shallower because momentum is not conserved well in PUBG
	//Assume you would only make angle steeper than current, but less steep than angle to deploy location
	//so array gets smaller as angle gets steeper
	

	//find min with for loop
	double temp_v_x = -cos(angle)*GRAVITY_CONST;
	double temp_v_y = -sin(angle)*GRAVITY_CONST - FALL_VY_CONST*dt;
	double temp_x = temp_v_x*dt + x;
	double temp_y = temp_v_y*dt + y;
	node* min_node = make_node(temp_x, temp_y);

	attach_node(location_node, min_node);

	

	//calc angle to location <- dont want to be steeper than straight at it due to assumption
	double angle_to_location;
	if(x <= 0)
		angle_to_location = PI/2;
	else
	    angle_to_location = atan(y/x); //angle
												 // ----     /*player*
												 //   angle /
												 //	      /
												 //     /
												 //  /
												 //--------------
	double min_time = path_before_chute(time+dt, temp_x, temp_y,  dt, angle_to_location, min_node); //angle stays say because we want to "keep course"							
	


	double angle_section_size;
	if (angle_to_location == PI/2)
		angle_section_size = 0;
	else
		angle_section_size = PI  / 10; //make sure to include horizontal TODO: dont test very small angles

	//connect location node to "min_node" <- We will start with first guess, then update if needed
	//test different angles, test from staright at it, to horiziontal
	if(angle_to_location != PI/2)
	for(double current_angle = angle_to_location - angle_section_size; current_angle > angle; current_angle = current_angle - angle_section_size) //Test different paths
	{
		//do  physics for current assumption
		double temp_vx = -cos(current_angle)*GRAVITY_CONST;
		double temp_vy = -sin(current_angle)*GRAVITY_CONST + FALL_VY_CONST;
		double temp_x = temp_vx*dt + x;
		double temp_y = temp_vy*dt + y;

		node* test_node = make_node(temp_x, temp_y);

		double test_time = path_before_chute(time+dt, temp_x, temp_y,  dt, current_angle, test_node); 
		if (test_time < min_time)
		{
			min_time = test_time;

			//connect to best result (update node) 
			
			node* min_node = copy_node(test_node); //make sure to use deep copy, because test node will be reset to use later
			attach_node(location_node, min_node);
		}
		free(test_node);
	}

	return min_time; //min_time is time to land if you took best path from current loc with current v_x and v_y
}

//make linked list to hold all previous x,y



void save_to_text(node* head)
{
	FILE *fp;
    fp = fopen("data.txt", "w");

    while(head != NULL)
    {
    	char line[50];
 
		sprintf(line,"%f %f\n", head->x, head->y);
    	fputs(line, fp);
    	if(head->next != NULL)
    		head = head->next;
    	else
    		head = NULL;
    }
    
    fclose(fp);
}


void make_graph()
{
	
}

int main()
{
	node* head = make_node(100,1051);
	double test = atan(1/1);
	printf("test: %f\n", test);
	double x = path_before_chute(0, 100, 1051, 1, 0, head);
	printf("Time: %f\n",x);
	save_to_text(head);
	return 0;

}