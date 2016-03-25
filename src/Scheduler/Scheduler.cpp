//
//  Scheduler.cpp
//  
//
//  Created by Ben Lengerich on 1/27/16.
//
//

#include "Scheduler.hpp"

#include <exception>
#include <iostream>
#include <unordered_map>
#include <node.h>
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <uv.h>

#ifdef BAZEL
#include "algorithm/ProximalGradientDescent.hpp"
//#include "algorithm/IterativeUpdate.hpp"
#include "algorithm/Algorithm.hpp"
#include "algorithm/AlgorithmOptions.hpp"
//#include "model/AdaMultiLasso.hpp"
//#include "model/GFlasso.h"
#include "model/lasso.hpp"
//#include "model/LinearRegression.hpp"
#include "model/Model.hpp"
#include "model/ModelOptions.hpp"
//#include "model/MultiPopLasso.hpp"
//#include "model/TreeLasso.hpp"
#include "Scheduler/Job.hpp"
#else
#include "../algorithm/ProximalGradientDescent.hpp"
//#include "../algorithm/IterativeUpdate.hpp"
#include "../algorithm/Algorithm.hpp"
#include "../algorithm/AlgorithmOptions.hpp"
//#include "../model/AdaMultiLasso.hpp"
//#include "../model/GFlasso.h"
#include "../model/lasso.hpp"
//#include "../model/LinearRegression.hpp"
#include "../model/Model.hpp"
#include "../model/ModelOptions.hpp"
//#include "../model/MultiPopLasso.hpp"
//#include "../model/TreeLasso.hpp"
#include "../Scheduler/Job.hpp"
#endif

using namespace std;

// Global static pointer used to ensure a single instance of this class.
Scheduler* Scheduler::s_instance = NULL;


//////////////////////////////////////////
// Constructors
//////////////////////////////////////////

Scheduler::Scheduler()
: next_algorithm_id(0)
, next_model_id(0)
, next_job_id(0) {
	algorithms_map = std::unordered_map<int, Algorithm*>();
	models_map = std::unordered_map<int, Model*>();
	jobs_map = std::unordered_map<int, Job_t*>();
}

Scheduler& Scheduler::operator=(Scheduler const& s) {
	return *Scheduler::Instance();
}

Scheduler* Scheduler::Instance() {	
	if (!s_instance) {	// Singleton
		s_instance = new Scheduler;
	}
	return s_instance;
}

/////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////

int Scheduler::newAlgorithm(const AlgorithmOptions_t& options) {
	Algorithm* my_algorithm;
	// Determine the type of algorithm to create.
	switch(options.type) {
		case proximal_gradient_descent:
			my_algorithm = new ProximalGradientDescent(options);
			break;
		/*case iterative_update:
			my_algorithm = new IterativeUpdate(options);
			break;*/
		default:
			return -1;
	}

	// Track it in this scheduler.
	int id = getNewAlgorithmId();
	if (id >= 0) {
		algorithms_map[id] = my_algorithm;
		return id;
	}

	delete my_algorithm;
	return -1;
}

int Scheduler::newModel(const ModelOptions_t& options) {
	Model* my_model;
	switch(options.type) {
		case linear_regression:
			my_model = new LinearRegression(options);
			break;
		/*case lasso:
			my_model = new Lasso(options);
			break;*/
		/*case ada_multi_lasso:
			my_model = new AdaMultiLasso(options);
			break;
		case gf_lasso:
			my_model = new GFlasso(options);
			break;
		case multi_pop_lasso:
			my_model = new MultiPopLasso(options);
			break;
		case tree_lasso:
			my_model = new TreeLasso(options);
			break;*/
		default:
			return -1;
	}

	int id = getNewModelId();
	if (id >= 0) {
		models_map[id] = my_model;
		return id;
	} else {
		delete my_model;
		return -1;
	}
}

int Scheduler::newJob(const JobOptions_t& options) {
	const int algorithm_id = options.algorithm_id;
	const int model_id = options.model_id;

	Job_t* my_job = new Job_t;
	my_job->job_id = getNewJobId();
	if (my_job->job_id >= 0) {
		my_job->algorithm = algorithms_map.find(algorithm_id)->second;
		my_job->model = models_map.find(model_id)->second;
		jobs_map[my_job->job_id] = my_job;
		return my_job->job_id;
	} else {
		delete my_job;
		return -1;
	}
}


bool Scheduler::startJob(Isolate* isolate, const Local<Function>& callback, const Local<Number>& job_id) {
	// TODO: check job_id here
	Job_t* job = Instance()->jobs_map[(int)job_id->Value()];
	job->request.data = job;
	job->callback.Reset(isolate, callback);
	job->job_id = (int)job_id->Value();

	// Start job async in a threadpool handled by libuv.
	uv_queue_work(uv_default_loop(), &job->request, trainAlgorithmThread, trainAlgorithmComplete);

	return true;
}


// Adds the job to the Scheduler's queue.
// Runs in libuv thread spawned by trainAlgorithmAsync
void trainAlgorithmThread(uv_work_t* req) {
	// Running in worker thread.
	Job_t* job = static_cast<Job_t*>(req->data);
	
	// Run algorithm here.
	job->algorithm->run(job->model);
	//job->results = job->algorithm->run(job->model);
}


// Handles packaging of algorithm results to return to the frontend.
// Called by libuv in event loop when async training completes.
void trainAlgorithmComplete(uv_work_t* req, int status) {
	// Runs in event loop when algorithm completes.
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope handleScope(isolate);

	Job_t* job = static_cast<Job_t*>(req->data);
	
	// Pack up the data here to be returned to JS - unclear what the format is
	Local<v8::Array> result_list = v8::Array::New(isolate);
	Handle<Value> argv[] = { result_list };

	// execute the callback
	Local<Function>::New(isolate, job->callback)->Call(
		isolate->GetCurrentContext()->Global(), 1, argv);
	job->callback.Reset();
	delete job;
}


double Scheduler::checkJob(const int job_id) {
	return Instance()->jobs_map[job_id]->algorithm->getProgress();
}


bool Scheduler::cancelJob(const int job_num) {
	// TODO: How to cancel an algorithm while it's running?
	// remove from maps and queues.
	return false;
}


bool Scheduler::deleteAlgorithm(const int algorithm_id) {
	// TODO: Safety checks here
	delete algorithms_map[algorithm_id];
	algorithms_map.erase(algorithm_id);
	return true;
}


bool Scheduler::deleteModel(const int model_id) {
	// TODO: Safety checks
	delete models_map[model_id];
	models_map.erase(model_id);
	return true;
}


bool Scheduler::deleteJob(const int job_id) {
	// TODO: safety checks
	delete jobs_map[job_id];
	jobs_map.erase(job_id);
	return true;
}


////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////

int Scheduler::getNewModelId() {
	int retval = next_model_id;
	for (int i = 1; i < s_instance->kMaxModelId; i++) {
		int candidate_model_id = (s_instance->next_model_id + i) % s_instance->kMaxModelId;
		if (s_instance->models_map.count(candidate_model_id) == 0) {
			s_instance->next_model_id = candidate_model_id;
			return retval;
		}
	}
	return -1;
}


int Scheduler::getNewAlgorithmId() {
	int retval = next_algorithm_id;
	for (int i = 1; i < s_instance->kMaxAlgorithmId; i++) {
		int candidate_algorithm_id = (s_instance->next_algorithm_id + i) % s_instance->kMaxAlgorithmId;
		if (s_instance->algorithms_map.count(candidate_algorithm_id) == 0) {
			s_instance->next_algorithm_id = candidate_algorithm_id;
			return retval;
		}
	}
	return -1;
}


int Scheduler::getNewJobId() {
	int retval = next_job_id;
	for (int i = 1; i < s_instance->kMaxJobId; i++) {
		int candidate_job_id = (s_instance->next_job_id + i) % s_instance->kMaxJobId;
		if (s_instance->jobs_map.count(candidate_job_id) == 0) {
			s_instance->next_job_id = candidate_job_id;
			return retval;
		}
	}
	return -1;
}