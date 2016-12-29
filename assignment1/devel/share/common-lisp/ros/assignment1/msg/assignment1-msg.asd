
(cl:in-package :asdf)

(defsystem "assignment1-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "triangle" :depends-on ("_package_triangle"))
    (:file "_package_triangle" :depends-on ("_package"))
  ))