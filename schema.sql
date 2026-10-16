-- ========================================================
-- Cinema Ticket Booking System - Database Schema & Seed Data
-- ========================================================

CREATE DATABASE IF NOT EXISTS `cinema_booking`;
USE `cinema_booking`;

-- Disable foreign key checks during setup
SET FOREIGN_KEY_CHECKS = 0;

-- 1. Customers Table
DROP TABLE IF EXISTS `customers`;
CREATE TABLE `customers` (
  `cust_id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(100) NOT NULL,
  `phone_no` BIGINT NOT NULL,
  PRIMARY KEY (`cust_id`),
  UNIQUE KEY `idx_phone_no` (`phone_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 2. Movies Table
DROP TABLE IF EXISTS `movies`;
CREATE TABLE `movies` (
  `movie_id` INT NOT NULL AUTO_INCREMENT,
  `title` VARCHAR(255) NOT NULL,
  `language` VARCHAR(50) DEFAULT NULL,
  `duration_mins` INT DEFAULT NULL,
  PRIMARY KEY (`movie_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 3. Screens Table
DROP TABLE IF EXISTS `screens`;
CREATE TABLE `screens` (
  `screen_id` INT NOT NULL AUTO_INCREMENT,
  `screen_number` INT NOT NULL,
  `silver_count` INT DEFAULT 0,
  `gold_count` INT DEFAULT 0,
  `platinum_count` INT DEFAULT 0,
  PRIMARY KEY (`screen_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 4. Seats Table
DROP TABLE IF EXISTS `seats`;
CREATE TABLE `seats` (
  `seat_id` INT NOT NULL AUTO_INCREMENT,
  `screen_id` INT NOT NULL,
  `seat_number` INT NOT NULL,
  `seat_type` VARCHAR(20) NOT NULL,
  PRIMARY KEY (`seat_id`),
  KEY `idx_seats_screen` (`screen_id`),
  CONSTRAINT `fk_seats_screen` FOREIGN KEY (`screen_id`) REFERENCES `screens` (`screen_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 5. Shows Table
DROP TABLE IF EXISTS `shows`;
CREATE TABLE `shows` (
  `show_id` INT NOT NULL AUTO_INCREMENT,
  `movie_id` INT NOT NULL,
  `screen_id` INT NOT NULL,
  `start_time` INT NOT NULL,
  `end_time` INT NOT NULL,
  PRIMARY KEY (`show_id`),
  KEY `idx_shows_movie` (`movie_id`),
  KEY `idx_shows_screen` (`screen_id`),
  CONSTRAINT `fk_shows_movie` FOREIGN KEY (`movie_id`) REFERENCES `movies` (`movie_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_shows_screen` FOREIGN KEY (`screen_id`) REFERENCES `screens` (`screen_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 6. Show Seats Table (Tracks seat availability per show)
DROP TABLE IF EXISTS `show_seats`;
CREATE TABLE `show_seats` (
  `show_seat_id` INT NOT NULL AUTO_INCREMENT,
  `show_id` INT NOT NULL,
  `seat_id` INT NOT NULL,
  `seat_number` INT NOT NULL,
  `seat_type` VARCHAR(20) NOT NULL,
  `status` VARCHAR(20) DEFAULT 'available',
  PRIMARY KEY (`show_seat_id`),
  KEY `idx_show_seats_show` (`show_id`),
  KEY `idx_show_seats_seat` (`seat_id`),
  CONSTRAINT `fk_show_seats_show` FOREIGN KEY (`show_id`) REFERENCES `shows` (`show_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_show_seats_seat` FOREIGN KEY (`seat_id`) REFERENCES `seats` (`seat_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 7. Bookings Table
DROP TABLE IF EXISTS `bookings`;
CREATE TABLE `bookings` (
  `booking_id` INT NOT NULL AUTO_INCREMENT,
  `cust_id` INT NOT NULL,
  `show_id` INT NOT NULL,
  `total_amount` INT NOT NULL,
  `booking_status` TINYINT(1) DEFAULT 1,
  `booked_at` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`booking_id`),
  KEY `idx_bookings_customer` (`cust_id`),
  KEY `idx_bookings_show` (`show_id`),
  CONSTRAINT `fk_bookings_customer` FOREIGN KEY (`cust_id`) REFERENCES `customers` (`cust_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_bookings_show` FOREIGN KEY (`show_id`) REFERENCES `shows` (`show_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 8. Booking Seats Junction Table
DROP TABLE IF EXISTS `booking_seats`;
CREATE TABLE `booking_seats` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `booking_id` INT NOT NULL,
  `show_seat_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_booking_seats_booking` (`booking_id`),
  KEY `idx_booking_seats_show_seat` (`show_seat_id`),
  CONSTRAINT `fk_booking_seats_booking` FOREIGN KEY (`booking_id`) REFERENCES `bookings` (`booking_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_booking_seats_show_seat` FOREIGN KEY (`show_seat_id`) REFERENCES `show_seats` (`show_seat_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 9. Payments Table
DROP TABLE IF EXISTS `payments`;
CREATE TABLE `payments` (
  `payment_id` INT NOT NULL AUTO_INCREMENT,
  `booking_id` INT NOT NULL,
  `amount` INT NOT NULL,
  `payment_method` VARCHAR(20) DEFAULT NULL,
  `paid_at` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`payment_id`),
  KEY `idx_payments_booking` (`booking_id`),
  CONSTRAINT `fk_payments_booking` FOREIGN KEY (`booking_id`) REFERENCES `bookings` (`booking_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

SET FOREIGN_KEY_CHECKS = 1;

-- ========================================================
-- Seed Initial Data
-- ========================================================

-- Insert Sample Movies
INSERT INTO `movies` (`movie_id`, `title`, `language`, `duration_mins`) VALUES
(1, 'Interstellar', 'English', 3),
(2, 'Swapped', 'English', 2),
(3, 'Inception', 'English', 3)
ON DUPLICATE KEY UPDATE `title`=VALUES(`title`);

-- Insert Sample Screens (Screen 1: 5 Silver, 5 Gold, 5 Platinum; Screen 2: 6 each)
INSERT INTO `screens` (`screen_id`, `screen_number`, `silver_count`, `gold_count`, `platinum_count`) VALUES
(1, 1, 5, 5, 5),
(2, 2, 6, 6, 6)
ON DUPLICATE KEY UPDATE `screen_number`=VALUES(`screen_number`);

-- Insert Sample Seats for Screen 1 (15 seats total)
INSERT INTO `seats` (`seat_id`, `screen_id`, `seat_number`, `seat_type`) VALUES
(1, 1, 1, 'Silver'), (2, 1, 2, 'Silver'), (3, 1, 3, 'Silver'), (4, 1, 4, 'Silver'), (5, 1, 5, 'Silver'),
(6, 1, 6, 'Gold'),   (7, 1, 7, 'Gold'),   (8, 1, 8, 'Gold'),   (9, 1, 9, 'Gold'),   (10, 1, 10, 'Gold'),
(11, 1, 11, 'Platinum'), (12, 1, 12, 'Platinum'), (13, 1, 13, 'Platinum'), (14, 1, 14, 'Platinum'), (15, 1, 15, 'Platinum')
ON DUPLICATE KEY UPDATE `seat_type`=VALUES(`seat_type`);

-- Insert Sample Seats for Screen 2 (18 seats total)
INSERT INTO `seats` (`seat_id`, `screen_id`, `seat_number`, `seat_type`) VALUES
(16, 2, 1, 'Silver'), (17, 2, 2, 'Silver'), (18, 2, 3, 'Silver'), (19, 2, 4, 'Silver'), (20, 2, 5, 'Silver'), (21, 2, 6, 'Silver'),
(22, 2, 7, 'Gold'),   (23, 2, 8, 'Gold'),   (24, 2, 9, 'Gold'),   (25, 2, 10, 'Gold'),  (26, 2, 11, 'Gold'),  (27, 2, 12, 'Gold'),
(28, 2, 13, 'Platinum'), (29, 2, 14, 'Platinum'), (30, 2, 15, 'Platinum'), (31, 2, 16, 'Platinum'), (32, 2, 17, 'Platinum'), (33, 2, 18, 'Platinum')
ON DUPLICATE KEY UPDATE `seat_type`=VALUES(`seat_type`);

-- Insert Sample Shows
INSERT INTO `shows` (`show_id`, `movie_id`, `screen_id`, `start_time`, `end_time`) VALUES
(1, 1, 1, 9, 12),
(2, 2, 2, 10, 12),
(3, 3, 1, 2, 5),
(4, 2, 2, 1, 3)
ON DUPLICATE KEY UPDATE `start_time`=VALUES(`start_time`);

-- Insert Show Seats for Show 1 (Screen 1)
INSERT INTO `show_seats` (`show_seat_id`, `show_id`, `seat_id`, `seat_number`, `seat_type`, `status`)
SELECT (s.seat_id), 1, s.seat_id, s.seat_number, s.seat_type, 'available'
FROM `seats` s WHERE s.screen_id = 1
ON DUPLICATE KEY UPDATE `status`=VALUES(`status`);

-- Insert Show Seats for Show 2 (Screen 2)
INSERT INTO `show_seats` (`show_seat_id`, `show_id`, `seat_id`, `seat_number`, `seat_type`, `status`)
SELECT (15 + (s.seat_id - 15)), 2, s.seat_id, s.seat_number, s.seat_type, 'available'
FROM `seats` s WHERE s.screen_id = 2
ON DUPLICATE KEY UPDATE `status`=VALUES(`status`);
