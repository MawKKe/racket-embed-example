#lang racket

(provide omg)
(provide hello-repl)

(define (omg) 42)

(define (hello-repl)
  (println (format 
             "Hello REPL! This source file was compiled embedded into the boot image! (omg) => ~a"
             (omg))))
