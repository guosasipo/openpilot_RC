import tensorflow as tf
import scipy.misc
import model
import cv2
import glob
from subprocess import call

sess = tf.compat.v1.InteractiveSession()
saver = tf.compat.v1.train.Saver()
saver.restore(sess, "save/model.ckpt")

img = cv2.imread('steering_wheel_image.jpg',0)
rows,cols = img.shape

smoothed_angle = 0

folder_name = "training_images/"
xs=[]

# Read files
for filename in glob.glob(folder_name + "*.jpg"):
    just_filename = filename[len(folder_name):]
    xs.append(folder_name+just_filename)

i = 0
while(cv2.waitKey(10) != ord('q')):
    full_image = scipy.misc.imread(xs[i], mode="RGB")
    image = scipy.misc.imresize(full_image[-150:], [66, 200]) / 255.0
    degrees = model.y.eval(feed_dict={model.x: [image], model.keep_prob: 1.0})[0][0] * 180.0 / scipy.pi
#    call("clear")
    #print("Predicted steering angle: " + str(degrees) + " degrees")
    cv2.imshow("frame", cv2.cvtColor(full_image, cv2.COLOR_RGB2BGR))
    #make smooth angle transitions by turning the steering wheel based on the difference of the current angle
    #and the predicted angle
    smoothed_angle += 0.2 * pow(abs((degrees - smoothed_angle)), 2.0 / 3.0) * (degrees - smoothed_angle) / abs(degrees - smoothed_angle)
    M = cv2.getRotationMatrix2D((cols/2,rows/2),-smoothed_angle,1)
    dst = cv2.warpAffine(img,M,(cols,rows))
    cv2.imshow("steering wheel", dst)
    i += 1

    str_angle = str(smoothed_angle)
    str_angle.zfill(3)

cv2.destroyAllWindows()
