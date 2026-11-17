-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Nov 18, 2026 at 12:17 AM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `digipump`
--

-- --------------------------------------------------------

--
-- Table structure for table `tbl_citizen`
--

CREATE TABLE `tbl_citizen` (
  `c_id` int(11) NOT NULL,
  `f_name` varchar(60) NOT NULL,
  `l_name` varchar(60) NOT NULL,
  `phone` varchar(20) NOT NULL,
  `u_id_fk` int(11) NOT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `tbl_citizen`
--

INSERT INTO `tbl_citizen` (`c_id`, `f_name`, `l_name`, `phone`, `u_id_fk`, `created_at`) VALUES
(1, 'Nihal', 'Tariq', '38439473743', 2, '2026-11-17 23:26:06'),
(3, 'Test', 'User', '03001234567', 7, '2026-11-18 04:15:21');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_employees`
--

CREATE TABLE `tbl_employees` (
  `e_id` int(11) NOT NULL,
  `f_name` varchar(60) NOT NULL,
  `l_name` varchar(60) NOT NULL,
  `phone` varchar(20) NOT NULL,
  `e_status` varchar(20) NOT NULL,
  `u_id_fk` int(11) NOT NULL,
  `s_id_fk` int(11) NOT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `tbl_owners`
--

CREATE TABLE `tbl_owners` (
  `o_id` int(11) NOT NULL,
  `f_name` varchar(60) NOT NULL,
  `l_name` varchar(60) NOT NULL,
  `phone` varchar(20) NOT NULL,
  `u_id_fk` int(11) NOT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `tbl_owners`
--

INSERT INTO `tbl_owners` (`o_id`, `f_name`, `l_name`, `phone`, `u_id_fk`, `created_at`) VALUES
(1, 'nihal', 'tariq', '03222138707', 1, '2026-11-17 21:18:03'),
(2, 't', 't', '6', 6, '2026-11-18 00:58:03');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_stations`
--

CREATE TABLE `tbl_stations` (
  `s_id` int(11) NOT NULL,
  `name` varchar(60) NOT NULL,
  `license` varchar(30) NOT NULL,
  `adress` text NOT NULL,
  `city` varchar(20) NOT NULL,
  `s_status` varchar(20) NOT NULL,
  `o_id_fk` int(11) NOT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `tbl_stations`
--

INSERT INTO `tbl_stations` (`s_id`, `name`, `license`, `adress`, `city`, `s_status`, `o_id_fk`, `created_at`) VALUES
(1, 'PSO', 'PS-102', 'North Karachi', 'KARACHI', 'Active', 1, '2026-11-18 01:04:49');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_user`
--

CREATE TABLE `tbl_user` (
  `u_id` int(11) NOT NULL,
  `username` varchar(60) NOT NULL,
  `email` varchar(60) NOT NULL,
  `userpass` varchar(255) NOT NULL,
  `role` varchar(20) NOT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `tbl_user`
--

INSERT INTO `tbl_user` (`u_id`, `username`, `email`, `userpass`, `role`, `created_at`) VALUES
(1, 'pump01', 'pump01@gmail.com', 'pbkdf2_sha256$600000$bf298b70090e60e5a73623a9a00ef5c4$90eb009f5ec21fb8407a1bcf99e1ede59e6a9e7f0b4896a2a1748c99d5ae02bb', 'Owner', '2026-11-17 21:18:03'),
(2, 'nihaltariq143', 'nihaltariq143@gmail.com', 'pbkdf2_sha256$600000$9a7dbf8f904a95d0ae1b034e1993e8dc$e72e43fbe96beaa5353322d1f8ca3a7eb2ee35c41d2ddd9443308c869545d18e', 'Citizen', '2026-11-17 23:26:06'),
(4, 'admin', 'admin@gmail.com', 'pbkdf2_sha256$600000$a5285e1e665e61f29c811e02267b7024$738f6119cf7b18dc3d6582076f77fed68923472e878668611f2ba97ef2a74486', 'Admin', '2026-11-17 23:30:00'),
(5, 'admin2', 'admin2@gmail.com', 'pbkdf2_sha256$600000$dc26a89f8e416cf4590b9ef532507ed3$bd0c182004dd4587e9f15e75c15c372edd28522b49291937c7aa08b8df813826', 'Admin', '2026-11-18 00:57:20'),
(6, 'pump02', 'pump02@gmail.com', 'pbkdf2_sha256$600000$7238b1c6208eec9761279d9f6c717a7a$85f66b35bac444f23ce5ebf8cd86b6cc393132fae8c7ac4214142ad53f96d3f2', 'Owner', '2026-11-18 00:58:03'),
(7, 'testuser01', 'test01@example.com', 'pbkdf2_sha256$600000$f7f776fa424b21eafb2b2602b959e9cf$7de3653bfc0368ba7aa4c0588e8833c509b791ce5bb79fbbb3a61f29e15a2ded', 'Citizen', '2026-11-18 04:15:21');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `tbl_citizen`
--
ALTER TABLE `tbl_citizen`
  ADD PRIMARY KEY (`c_id`),
  ADD KEY `FK_citizen_user` (`u_id_fk`);

--
-- Indexes for table `tbl_employees`
--
ALTER TABLE `tbl_employees`
  ADD PRIMARY KEY (`e_id`),
  ADD KEY `FK_employee_user` (`u_id_fk`),
  ADD KEY `FK_employee_station` (`s_id_fk`);

--
-- Indexes for table `tbl_owners`
--
ALTER TABLE `tbl_owners`
  ADD PRIMARY KEY (`o_id`),
  ADD KEY `FK_owner_user` (`u_id_fk`);

--
-- Indexes for table `tbl_stations`
--
ALTER TABLE `tbl_stations`
  ADD PRIMARY KEY (`s_id`),
  ADD UNIQUE KEY `license` (`license`),
  ADD KEY `FK_station_owner` (`o_id_fk`);

--
-- Indexes for table `tbl_user`
--
ALTER TABLE `tbl_user`
  ADD PRIMARY KEY (`u_id`),
  ADD UNIQUE KEY `username` (`username`),
  ADD UNIQUE KEY `email` (`email`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `tbl_citizen`
--
ALTER TABLE `tbl_citizen`
  MODIFY `c_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `tbl_employees`
--
ALTER TABLE `tbl_employees`
  MODIFY `e_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `tbl_owners`
--
ALTER TABLE `tbl_owners`
  MODIFY `o_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `tbl_stations`
--
ALTER TABLE `tbl_stations`
  MODIFY `s_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `tbl_user`
--
ALTER TABLE `tbl_user`
  MODIFY `u_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `tbl_citizen`
--
ALTER TABLE `tbl_citizen`
  ADD CONSTRAINT `FK_citizen_user` FOREIGN KEY (`u_id_fk`) REFERENCES `tbl_user` (`u_id`) ON DELETE CASCADE;

--
-- Constraints for table `tbl_employees`
--
ALTER TABLE `tbl_employees`
  ADD CONSTRAINT `FK_employee_station` FOREIGN KEY (`s_id_fk`) REFERENCES `tbl_stations` (`s_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `FK_employee_user` FOREIGN KEY (`u_id_fk`) REFERENCES `tbl_user` (`u_id`) ON DELETE CASCADE;

--
-- Constraints for table `tbl_owners`
--
ALTER TABLE `tbl_owners`
  ADD CONSTRAINT `FK_owner_user` FOREIGN KEY (`u_id_fk`) REFERENCES `tbl_user` (`u_id`) ON DELETE CASCADE;

--
-- Constraints for table `tbl_stations`
--
ALTER TABLE `tbl_stations`
  ADD CONSTRAINT `FK_station_owner` FOREIGN KEY (`o_id_fk`) REFERENCES `tbl_owners` (`o_id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
