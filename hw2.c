#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

asmlinkage long sys_hello(void){
	printk("Hello, World!\n"); 
	return 0;
}

asmlinkage long sys_set_weight(int weight){
	if(weight < 0 ) return -EINVAL;
	current->weight = weight;
	return 0;
}

asmlinkage long sys_get_weight(void){
	if(current->weight < 0 ) return 0;
	else return current->weight;
}

long get_leaf_sum(struct task_struct *task){
	struct list_head child_list, *iter;
	child_list = current->children;
	if(list_entry(iter, struct task_struct, children) == NULL) return -EINVAL;
	long sum = 0;
	list_for_each(iter, &child_list){
		task = list_entry(iter, struct task_struct, children);
		if(iter->next == NULL) sum+= task->weight;
		else{
			long res = get_leaf_sum(list_entry(iter->next, struct task_struct, children));
			if(res != -EINVAL){
				sum += res;
			}
		}
 	}
	return sum;
}

asmlinkage long sys_get_leaf_children_sum(void){
	return get_leaf_sum(current);
}

asmlinkage long sys_get_heaviest_ancestor(void){
	struct task_struct *task;
	task = current;
	pid_t pid = task_pid(current);
	long max_weight = current->weight;
	while(task->parent != NULL){
		task = task->parent;
		if(task->weight > max_weight){
			max_weight = task->weight;
			pid = task_pid(task);
		}
	}
	return pid;
}
