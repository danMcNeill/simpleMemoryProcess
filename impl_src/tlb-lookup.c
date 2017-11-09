#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "stats.h"
#include "macros.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, attempts to access the
 * current page table via cpu_pagetable_lookup().
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */

pfn_t tlb_lookup(vpn_t vpn, int write)
{
	int i = 0;
	pfn_t pfn;

	/*
	 * TASK 2a : Implement TLB lookup
	 * Search the TLB for the given VPN. Make sure to increment tlbhits counter if
	 * it was a hit!
	 */
	


	/* 
	 * STEPS: 
	 * Iterate through the TLB entries: 
	 *    If the entry is valid and the virtual page number is the same
	 * as what is provided as input, then:
	 *        Set the used bit (because we are using it)
	 * 	  If this is a write access, also set the dirty bit. 
	 * 	  We have found the physical page, so we are done. Return the physical page. 
	 */

	tlbe_t* tempTlb = tlb;

	for(i=0; i<tlb_size; i++) {
		if((vpn == tempTlb[i].vpn) && IS_SET((tempTlb[i].flags), (VALID))){ /*if valid and vpn right*/
			SET_BIT((tempTlb[i].flags), (USED));
			if(write == 1) {
				SET_BIT((tempTlb[i].flags), (DIRTY));
			}
			pfn = tempTlb[i].pfn;
			tlbhits_count++;
			return pfn;
		}
	}
			
	/* TASK 2b: If it was a TLB miss, call pagetable_lookup to obtain the physical page. */

	pfn = pagetable_lookup(vpn, write);


	/* TASK 2c: Evict an invalid entry and update the TLB with the new page */

	tempTlb = tlb;
	for(i=0; i<tlb_size; i++) {
		if(!(IS_SET((tempTlb[i].flags), (VALID)))) { /*if not valid, overwrite */
			tempTlb[i].vpn = vpn; /* changes virutal address value */
			tempTlb[i].pfn = pfn; /* changes physical number */
			SET_BIT((tempTlb[i].flags), (VALID)); /* sets valid bit to be valid */
			SET_BIT((tempTlb[i].flags), (USED)); /* sets used bit to be valid */
			if(write == 1) {
				SET_BIT((tempTlb[i].flags), (DIRTY));
			}
			return pfn;
		}
	}

	

	
	/* TASK 2d: If no invalid entry was found, we must kick out an unused entry */
	/* STEPS: 
	 * For each entry in the TLB:
	 *   If the entry is unused:
	 *      set the virtual and physical page mapping and mark the entry as used and valid.
	 * 	we have successfully completed the eviction process. we are done. Return the physical page. 
	 *   else:
	 * 	mark the entry as unused, so we can kick it out the next time it is encountered. This is called performing a clock-sweep, because we are mimicking a movement of time. 
	 * Repeat the for STEPS till we find a victim entry to kick out. 
	 */

	/*if no invalid entry found */
	tempTlb = tlb;
	for(i=0; i<tlb_size; i++) {
		if(!(IS_SET((tempTlb[i].flags), (USED)))) { /* if entry is unused */
			tempTlb[i].vpn = vpn;
			tempTlb[i].pfn = pfn;
			SET_BIT((tempTlb[i].flags), (USED));
			SET_BIT((tempTlb[i].flags), (VALID));
			if(write == 1) {
				SET_BIT((tempTlb[i].flags), (DIRTY));
			}
			return pfn;
		}
		else {
			CLEAR_BIT((tempTlb[i].flags), (USED));
		}
	}



	return pfn;
}

