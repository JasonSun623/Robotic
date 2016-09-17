; Auto-generated. Do not edit!


(cl:in-package assignment1-msg)


;//! \htmlinclude triangle.msg.html

(cl:defclass <triangle> (roslisp-msg-protocol:ros-message)
  ((sideLength
    :reader sideLength
    :initarg :sideLength
    :type cl:float
    :initform 0.0)
   (clw
    :reader clw
    :initarg :clw
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass triangle (<triangle>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <triangle>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'triangle)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name assignment1-msg:<triangle> is deprecated: use assignment1-msg:triangle instead.")))

(cl:ensure-generic-function 'sideLength-val :lambda-list '(m))
(cl:defmethod sideLength-val ((m <triangle>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader assignment1-msg:sideLength-val is deprecated.  Use assignment1-msg:sideLength instead.")
  (sideLength m))

(cl:ensure-generic-function 'clw-val :lambda-list '(m))
(cl:defmethod clw-val ((m <triangle>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader assignment1-msg:clw-val is deprecated.  Use assignment1-msg:clw instead.")
  (clw m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <triangle>) ostream)
  "Serializes a message object of type '<triangle>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'sideLength))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'clw) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <triangle>) istream)
  "Deserializes a message object of type '<triangle>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'sideLength) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'clw) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<triangle>)))
  "Returns string type for a message object of type '<triangle>"
  "assignment1/triangle")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'triangle)))
  "Returns string type for a message object of type 'triangle"
  "assignment1/triangle")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<triangle>)))
  "Returns md5sum for a message object of type '<triangle>"
  "91bb36a3b1793754cf4d40aa5e809a34")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'triangle)))
  "Returns md5sum for a message object of type 'triangle"
  "91bb36a3b1793754cf4d40aa5e809a34")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<triangle>)))
  "Returns full string definition for message of type '<triangle>"
  (cl:format cl:nil "float32 sideLength~%bool clw~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'triangle)))
  "Returns full string definition for message of type 'triangle"
  (cl:format cl:nil "float32 sideLength~%bool clw~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <triangle>))
  (cl:+ 0
     4
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <triangle>))
  "Converts a ROS message object to a list"
  (cl:list 'triangle
    (cl:cons ':sideLength (sideLength msg))
    (cl:cons ':clw (clw msg))
))
