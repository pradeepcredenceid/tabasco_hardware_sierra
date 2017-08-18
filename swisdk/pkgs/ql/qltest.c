/*
 * Name: qltest.c
 *
 * Purpose: Provides a main function that calls the ql functions directly
 *          from an executable on the development machine to exercise the
 *          package
 *
 * Note:    None
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "aa/aaglobal.h"
#include "ql/qludefs.h"

/* Test Structures */
struct testuserdata
{
    int nodenum;
    struct qllink link;
};

/* Sample FIFO queue */
void fifoqueue(void)
{
    struct testuserdata *headerp;
    int i;
    struct testuserdata *datap;

    /* 
     * 1. CREATE QUEUE HEADER 
     */

    /* allocate memory for header */
    headerp = (struct testuserdata *)malloc(sizeof (struct testuserdata));

    /* initialize queue header */
    qlinitlnk(&headerp->link, NULL);

    /*
     * 2. ADD LINKS TO THE END OF THE QUEUE 
     */

    /* add  5 links to the end of queue  */
    for (i = 1; i <= 5; i++)
    {
        /* allocate memory for queue link */
        datap = (struct testuserdata *)malloc(sizeof (struct testuserdata));

        /*  filling struct with important information to be queued */
        datap->nodenum = i;

        /* initialize queue link */
        qlinitlnk(&datap->link, datap);

        /* add link to head of queue */
        qladdprev(&headerp->link, &datap->link);
    }

    /*
     * 3. ADD LINK TO THE FRONT OF THE QUEUE
     */

    /* allocate memory for queue link */
    datap = (struct testuserdata *)malloc(sizeof (struct testuserdata));

    /*  filling struct with important information to be queued */
    datap->nodenum = 99;

    /* initialize queue link */
    qlinitlnk(&datap->link, datap);

    /* add link to head of queue */
    qladdnext(&headerp->link, &datap->link);

    /*
     * 4. LOOP THROUGH ALL LINKS ON THE QUEUE 
     *    STARTING AT THE FRONT
     */

    /* get pointer to first link in the queue */
    datap = (struct testuserdata *)qlgetnext(&headerp->link);

    printf("QUEUE LINKS");
    /* loop through queue */
    while(1)
    {
        /* if this is not the header link then the next data is... */
        if (datap != NULL)
        {
            printf("...%d", datap->nodenum);
        }
        /* completed loop through all links */
        else
        {
            printf("...end of queue\n");
            break;
        }
        datap = (struct testuserdata *)qlgetnext(&datap->link);
    }
    
    /*
     * 5. REMOVE LINKS FROM THE FRONT OF THE QUEUE
     */

    printf("REMOVING LINKS");
    /* loop through queue */
    while(1)
    {
        /* remove first queue link from the queue */
        datap = (struct testuserdata *)qlremprev(&headerp->link);

        /* if this is not the header link then the data removed is... */
        if (datap != NULL)
        {
            printf("...%d", datap->nodenum);
            free((void *)datap);
        }
        /* the header is the only link left in the queue */
        else
        {    
            printf("...empty queue\n");
            break;
        }
    }
}
    

/* Test Functions */

void addtofront(struct testuserdata *headerp)
{
    struct testuserdata *datap;
    int i;

    printf("add links to front");
    /* add links to queue  */
    for (i = 1; i <= 5; i++)
    {
        /* allocate memory for queue link */
        datap = (struct testuserdata *)malloc(sizeof (struct testuserdata));

        /*  filling struct with important information to be queued */
        datap->nodenum = i;

        /* initialize queue link */
        qlinitlnk(&datap->link, datap);

        /* add link to head of queue */
        qladdprev(&headerp->link, &datap->link);
        printf("...%d", datap->nodenum);
    }
    printf("\n");
}

void printfront2back(struct testuserdata *headerp)
{
    struct testuserdata *datap;

    printf("Queue from front to back");
    /* get pointer to first link in the queue */
    datap = (struct testuserdata *)qlgetnext(&headerp->link);

    /* loop through queue */
    while(1)
    {
        /* if this is not the header link then the next data is... */
        if (datap != NULL)
        {
            printf("...%d", datap->nodenum);
        }
        /* completed loop through all links */
        else
        {
            printf("...end of queue\n");
            break;
        }
        datap = (struct testuserdata *)qlgetprev(&datap->link);
    }

}

void printback2front(struct testuserdata *headerp)
{
    struct testuserdata *datap;

    printf("Queue from back to front");
    /* get pointer to last link in queue */
    datap = (struct testuserdata *)qlgetprev(&headerp->link);

    /* loop through queue */
    while(1)
    {
        /* if this is not the header link then the next data is... */
        if (datap != NULL)
        {
            printf("...%d", datap->nodenum);
        }
        /* completed loop through all links */
        else
        {
            printf("...end of queue\n");
            break;
        }
        datap = (struct testuserdata *)qlgetprev(&datap->link);
    }

}
void clearqueue(struct testuserdata *headerp)
{
    struct testuserdata *datap;

    printf("removing link from back to front");
    while(1)
    {
        /* remove last queue link from the queue */
        datap = (struct testuserdata *)qlremprev(&headerp->link);

        /* if this is not the header link then the data removed is... */
        if (datap != NULL)
        {
            printf("...%d", datap->nodenum);
            free((void *)datap);
        }
        /* the header is the only link left in the queue */
        else
        {    
            printf("...empty queue\n");
            break;
        }
    }
}

void removeeven(struct testuserdata *headerp)
{
    struct testuserdata *nextdatap;
    struct testuserdata *datap;
    
    printf("remove even nodes\n");

    /* initialize queue pointer */
    datap = headerp;

    /* get next data */
    nextdatap = (struct testuserdata *)qlgetnext(&headerp->link);

    /* loop through queue */
    while(1)
    {
        /* if nodenum is even */
        if ((nextdatap->nodenum/2)*2 == nextdatap->nodenum)
        {
            /* remove node from queue */
            qlremnext(&datap->link);
        }
        else
        {
            /* forward queue pointer */
            datap = nextdatap;
        }

        /* get next data */
        nextdatap = (struct testuserdata *)qlgetnext(&datap->link);

        /* if looped back to the header */
        if (nextdatap == NULL)
            break;
    }
}

void removeodd(struct testuserdata *headerp)
{
    struct testuserdata *nextdatap;
    struct testuserdata *datap;
    
    printf("remove odd nodes\n");

    /* initialize queue pointer */
    datap = headerp;

    /* get next data */
    nextdatap = (struct testuserdata *)qlgetnext(&headerp->link);

    /* loop through queue */
    while(1)
    {
        /* if nodenum is even */
        if ((nextdatap->nodenum/2)*2 != nextdatap->nodenum)
        {
            /* remove node from queue */
            qlremnext(&datap->link);
        }
        else
        {
            /* forward queue pointer */
            datap = nextdatap;
        }

        /* get next data */
        nextdatap = (struct testuserdata *)qlgetnext(&datap->link);

        /* if looped back to the header */
        if (nextdatap == NULL)
            break;
    }
}

void queuetest(void)
{
    struct testuserdata *headerp;

    /* allocate memory for header */
    headerp = (struct testuserdata *)malloc(sizeof (struct testuserdata));

    /* initialize queue header */
    qlinitlnk(&headerp->link, NULL);

    /* add links to queue */
    addtofront(headerp);

    /* loop through queue */
    printfront2back(headerp);

    /*remove even nodenums from queue */
    removeeven(headerp);

    /* loop through queue */
    printfront2back(headerp);

    /* clear queue */
    clearqueue(headerp);

    /* loop through queue */
    printfront2back(headerp);

    /* add links to queue */
    addtofront(headerp);

    /* loop through queue */
    printback2front(headerp);

    /*remove odd nodenums from queue */
    removeodd(headerp);

    /* loop through queue */
    printback2front(headerp);

    /* clear queue */
    clearqueue(headerp);

    /* loop through queue */
    printback2front(headerp);

    free((void *)headerp);
}
    
int main(int argc, char* argv[])
{
    printf("Testing Queue Links!\n");
    queuetest();

    printf("\nFIFO Queue test\n");
    fifoqueue();
     
    return 0;
}
