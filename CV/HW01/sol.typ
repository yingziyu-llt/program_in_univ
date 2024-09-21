#import "@local/PKU-Typst-Template:0.1.0": 通用作业

#show: 通用作业.config.with(
  course_name: "计算机视觉",
  student_id: 2300012154,
  student_name: "林乐天",
  hw_no: 1,
)
#set text(size: 20pt)
= Short questions
#set text(size: 14pt)
#通用作业.hw_prob("Homogeneous coordination")[
  In the Cartesian coordinate system, a point in $N$ -dimensional space is represented by $N$ coordinates as $P = (x_1, x_2, . . . , x_N )$. In homogeneous coordinates, the same point is represented with $N + 1$ coordinates as $P' = (x_1, x_2, . . . , x_N , w)$, where $w$ is a non-zero real number representing the weight or scale of the point. For simplicity, $P ′$ can be rewritten as $(x_1', x_2', . . . , x_N' , 1)$

Please:
+ Derive the mathematical relationship between $x_i$ and $x'_i$. (3 points)
+ Explain at least two advantages of using homogeneous coordinates, and why they are adopted in computational photography. (7 points)
][
  #set enum(numbering: "Sub-problem (1)")
  + For $x_i$,it equals to $x'_i/w$. So the relationship is $x_i = x'_i/w$.
  + Advantages:
    - It makes it easier to perform geometric transformations like translation, for we can simply multiply it with a matrix $ mat(I_"3x3",T;0,1) $
    - It can show points in infinity, for it can represent points at infinity by setting the $w$ coordinate to 0.
    - It enables the processing of lines and points in the same way, as it is more convenient to deal with.
]
#通用作业.hw_prob("Dolly zoom")[A dolly zoom (also known as a Hitchcock shot or Vertigo shot) is an in-camera effect that appears to undermine normal visual perception. This technique can produce striking effects, where the background appears to expand rapidly in size and detail, overwhelming the foreground, or where the foreground swells to dominate its surroundings, depending on how the dolly zoom is performed. Figure 1 illustrates aclassic example of this effect. *Please explain the mechanics behind how this effect is achieved*.][  
  The dolly zoom effect is achieved by moving the camera while adjusting the focal length of the lens.

  During the process of moving the camera forward, adjust the focal length to zoom out, ensuring that the subject remains in focus while the field of view continually changes,and the main character's size remains constant. This creates the illusion of the background expanding and the foreground shrinking, creating an artistic effect of disorientation.
]

