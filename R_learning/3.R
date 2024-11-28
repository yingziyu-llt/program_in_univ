library(ggplot2)

# 读取数据
myData <- read_excel("~/Downloads/data.xlsx", sheet = 1)

# 线性回归
linerModel <- lm(c ~ A, myData)
slope <- coef(linerModel)[2]
intercept <- coef(linerModel)[1]

# 绘制散点图和回归线
plot(myData$A, myData$c, main="Fe(CN)3 A-c工作曲线", xlab="A", ylab="c(mg/ml)", col="blue", pch=16)
abline(linerModel, col="black")

# 添加文本注释
text(x=min(myData$A) + 0.065, y=max(myData$c) - 0.001, labels = paste("R^2=", round((summary(linerModel)$r.squared), 5)))
text(x=min(myData$A) + 0.074, y=max(myData$c) - 0.0005, labels = paste("slope=", signif(slope, digits=3)))
text(x=min(myData$A) + 0.11, y=max(myData$c), labels = paste("intercept=", signif(intercept, digits=3)))

# 计算并绘制一个点
calc_x1 <- 0.317 * slope + intercept
points(0.317, calc_x1, col="red", pch=19)

# 添加图例
legend("bottomright", legend=c("工作曲线使用的点", "工作曲线", "未知溶液"),
       col=c("blue", "black", "red"), pch=c(16, NA, 19), lty=c(0, 1, 0), bty="n")
