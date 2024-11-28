library(readxl)
library(ggplot2)
all_data = read.csv("./data.csv")
standard_1 = unlist(all_data[1])
standard_2 = unlist(all_data[2])
standard_ave = (standard_1 + standard_2) / 2.0
standard_ave = head(standard_ave,-2)
test1 = unlist(all_data[3])
test2 = unlist(all_data[4])
test_ave = (test1 + test2) / 2.0


standard_protein = 200 * c(20,16,12,8,4,0) / 20

df = data.frame(standard_ave,standard_protein)

ggplot(df,aes(x=standard_protein,y=standard_ave))+geom_point()+geom_smooth(method = "lm")

l = lm(standard_ave~standard_protein,data = df)

summary(l)

k = coefficients(l)[2]
b = coefficients(l)[1]

test_ave = (test1 + test2) / 2
test_protein = (test_ave - b)/k

test_protein = test_protein * 20 / 5
