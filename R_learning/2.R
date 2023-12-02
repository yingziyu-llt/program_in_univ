library(readxl)
library(ggplot2)
my_data <- read_excel("~/Downloads/data.xlsx", sheet = 1)
my_vector1 <- unlist(my_data[1])
my_vector2 <- unlist(my_data[2])

average <- (my_vector1 + my_vector2) / 2
data <- average[1:6]
e <- my_vector1 - my_vector2
maxn <- max(abs(e))
x_row <- 0:5
my_data <- data.frame(x_row,data)
linear_Model <- lm(data~x_row,my_data)
slope <- coef(linear_Model)[2]
intercept <- coef(linear_Model)[1]
calc_x1 <- (average[7] - intercept) / slope
calc_x2 <- (average[8] - intercept) / slope
calc_x1 <- calc_x1 * 25 / 2
calc_x2 <- calc_x2 * 5 / 2

plot(my_data$x_row, my_data$data, main="蛋白质含量和吸光度标准曲线", xlab="蛋白质含量(g)", ylab="吸光度")  
abline(linear_Model, col="red")
text(x=min(x_row) + 0.3,y=max(data),labels = paste("R^2=",round((summary(linear_Model)$r.squared),5)))
print(average)
print(my_vector1)
print(my_vector2)
