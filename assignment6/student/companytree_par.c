#include "companytree.h"

void traverse_util(tree *node) {

//write your parallel solution here

  if(node != NULL){
    traverse_util(node->left);
    traverse_util(node->right);
    node->work_hours = compute_workHours(node->data);
    top_work_hours[node->id] = node->work_hours;
  }
}

void process(tree *node) {
  node->work_hours = compute_workHours(node->data);
  top_work_hours[node->id] = node->work_hours;
}

void traverse(tree *node, int numThreads) {

#pragma omp parallel sections
  {
#pragma omp section
    {
      process(node);
      process(node->left);
      process(node->right);
      process(node->left->left);
      process(node->left->right);
      process(node->right->left);
      process(node->right->right);
    }
#pragma omp section
    traverse_util(node->left->left->left);
#pragma omp section
    traverse_util(node->left->left->right);
#pragma omp section
    traverse_util(node->left->right->left);
#pragma omp section
    traverse_util(node->left->right->right);
#pragma omp section
    traverse_util(node->right->left->left);
#pragma omp section
    traverse_util(node->right->left->right);
#pragma omp section
    traverse_util(node->right->right->left);
#pragma omp section
    traverse_util(node->right->right->right);
  }
}

