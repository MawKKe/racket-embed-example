#lang racket

(provide pythagoras)

(define (pythagoras a b)
  (sqrt (+ (* a a) (* b b))))
