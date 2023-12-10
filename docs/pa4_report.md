# Programming Assignment 4: Statistics Estimation Report

## Overall Plan Cost Estimation
The estimation of the overall plan cost is primarily focused on the computation of I/O and CPU costs for sequences of joins and base table accesses. The cost computation involves a simple yet effective formula that considers both the scan cost of individual tables and the join cost of combining these tables.

## Join Cost Estimation
Join cost estimation in this assignment utilizes a nested loops join approach. The implementation effectively calculates the I/O cost based on the scan cost of the tables involved in the join, and the CPU cost is derived from the product of the number of tuples in each table.

In the join cost estimation, the function estimateJoinCost calculates the total cost of a join between two tables. The cost comprises two components: I/O cost and CPU cost. The I/O cost is determined by adding the scan cost of the first table (cost1) to the product of the number of tuples in the first table (card1) and the scan cost of the second table (cost2). This reflects the nested loop join's nature, where each tuple in the outer table (first table) requires a full scan of the inner table (second table). The CPU cost is simply the product of the number of tuples in both tables (card1 * card2), representing the computational effort to process each tuple combination.

## Filter Selectivity Estimation
Filter selectivity estimation is implemented using a histogram-based approach. This method is particularly effective in estimating the selectivity for both equality and range expressions. The histogram is constructed to represent the distribution of values, allowing for a more accurate estimation of selectivity based on the query predicates.

In estimateSelectivity, the selectivity of a predicate over a column is estimated using a histogram-based approach. This function handles different types of predicates (equals, greater than, less than, etc.) and computes selectivity accordingly. If the value (v) in the predicate falls within the histogram range, the corresponding bucket is identified, and the selectivity is estimated based on the number of tuples in that bucket and the bucket width. This assumes a uniform distribution within each bucket. For range queries, the selectivity is calculated by summing up the fractions of relevant buckets. Special cases where the value falls outside the histogram range are also handled, returning either 0 or 1 as the selectivity based on the predicate type.

## Join Cardinality Estimation
The join cardinality estimation handles different scenarios including equality joins with and without primary keys, and range joins. The cardinality estimation is critical in predicting the size of the output from join operations, which in turn influences the overall cost estimation of the query plan.

The function estimateTableJoinCardinality estimates the number of tuples resulting from a join operation. It considers different scenarios: when one or both tables have primary keys and when there are non-equality joins (like range joins). For equality joins involving primary keys, the cardinality is limited by the non-primary key table's size. In the absence of primary keys, a heuristic is used, typically the maximum size of the two tables. For range joins, a fixed fraction (30% in this case) of the cross-product of the two tables' sizes is assumed, reflecting the typical larger output size for range joins compared to equality joins.

## Join Ordering
The implementation of join ordering is a key aspect of this assignment, involving the optimization of query plans. The method iteratively evaluates different combinations of join sequences and uses dynamic programming along with a caching mechanism to determine the most cost-effective join order.

The method orderJoins implements a dynamic programming approach to determine the optimal order of joins. It uses a PlanCache to store the best plans for subsets of joins. The function iteratively considers all subsets of the available joins, updating the best plan for each subset based on the cost. For each subset, it looks for a smaller subset and calculates the cost of adding one more join to this subset, selecting the combination with the minimum cost. This process is repeated until the best plan for the full set of joins is determined. The final join order ensures that each pair of adjacent joins in the plan shares at least one field, adhering to the requirement of a left-deep join plan.
## Challenges and Solutions
### Filter Selectivity Estimation: 
The challenge here was to accurately estimate selectivity for various predicates. The solution involved using a histogram-based approach which assumes a uniform distribution of values within each bucket.
### Join Ordering Optimization: 
The main challenge was to find the optimal join order for complex queries. The solution was to implement a dynamic programming approach that examines all subsets of join nodes, caching intermediate results to efficiently determine the best order.
## Conclusion
This assignment provided a comprehensive understanding of the complexities involved in estimating the costs associated with database query plans. The methodologies implemented for join cost, filter selectivity, and join ordering are instrumental in optimizing the performance of database systems.