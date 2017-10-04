library(foreach)
library(randomForest)
library(e1071) 

#Build two simple models, SVM and RandomForest, and see if we can house them in an R data structure
set.seed(10)  
y<-c(1:1000)  
x1<-c(1:1000)*runif(1000,min=0,max=2)  
x2<-(c(1:1000)*runif(1000,min=0,max=2))^2  
x3<-log(c(1:1000)*runif(1000,min=0,max=2))  

set.seed(10)  
#split data
all_data<-data.frame(y,x1,x2,x3)  
positions <- sample(nrow(all_data),size=floor((nrow(all_data)/4)*3))  
training<- all_data[positions,]  
testing<- all_data[-positions,]  

#svm model 
svm_fit<-svm(y~x1+x2+x3,data=training)  
#svm_predictions<-predict(svm_fit,newdata=testing)  
#error_svm<-sqrt((sum((testing$y-svm_predictions)^2))/nrow(testing))  

#Example Bagging function used on the svm
length_divisor<-6  
iterations<-5000  
predictions<-foreach(m=1:iterations,.combine=cbind) %do% {  
  training_positions <- sample(nrow(training), size=floor((nrow(training)/length_divisor)))  
  train_pos<-1:nrow(training) %in% training_positions  
  svm_fit<-svm(y~x1+x2+x3,data=training[train_pos,])  
  predict(svm_fit,newdata=testing)  
}  
svm2_predictions<-rowMeans(predictions)  
#svm performs worse after bagging
error_svm2<-sqrt((sum((testing$y-svm2_predictions)^2))/nrow(testing))    

#random forest model
rf_fit<-randomForest(y~x1+x2+x3,data=training,ntree=500)  
#rf_predictions<-predict(rf_fit,newdata=testing)

#ensemble with non bagged svm and random forest
#predictions<-(svm_predictions+rf_predictions)/2  
#error_ens_sr<-sqrt((sum((testing$y-predictions)^2))/nrow(testing))  

#test storing the models in an R data structure
model_container <- list(type=any)
model_container[[1]] <- svm_fit
model_container[[2]] <- rf_fit

#use predict method with svm model in our model_list
svm_predictions<-predict(model_container[[1]],newdata=testing)  
#MSE
error_svm<-sqrt((sum((testing$y-svm_predictions)^2))/nrow(testing))

#use predict method with rf model in out model_list
rf_predictions<-predict(model_container[[2]],newdata=testing)
#MSE
error_rf <- sqrt((sum((testing$y-rf_predictions)^2))/nrow(testing))

#ensemble with svm and rf models stored in the model_list
predictions<-(svm_predictions+rf_predictions)/2  
#MSE
error_ens_sr<-sqrt((sum((testing$y-predictions)^2))/nrow(testing))
